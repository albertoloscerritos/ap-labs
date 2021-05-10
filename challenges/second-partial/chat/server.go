// Copyright © 2016 Alan A. A. Donovan & Brian W. Kernighan.
// License: https://creativecommons.org/licenses/by-nc-sa/4.0/

// See page 254.
//!+

// Chat is a server that lets clients chat with each other.
package main

import (
	"bufio"
	"bytes"
	"flag"
	"fmt"
	"log"
	"net"
	"strings"
	"time"
)

//!+broadcaster
type client chan<- string // an outgoing message channel

type userStruct struct {
	userName string
	msg      string
}

type clientStruct struct {
	cliente        client
	connection     net.Conn
	connectedSince string
}

var (
	pre_line            = "irc-server > "
	admin               client
	fullUser            string
	direct              = make(chan userStruct)
	kick                = make(chan string)
	users               map[string]client
	connections         map[string]net.Conn
	entering            = make(chan clientStruct)
	leaving             = make(chan client)
	messages            = make(chan string)
	connections_history map[string]string
	kick_bool           = false
)

func broadcaster() {
	clients := make(map[client]bool) // all connected clients
	users = make(map[string]client)
	connections = make(map[string]net.Conn)
	connections_history = make(map[string]string)

	for {
		select {
		case msg := <-messages:
			// Broadcast incoming message to all
			// clients' outgoing message channels.
			for cli := range clients {
				cli <- msg
			}

		case priv := <-direct:
			users[priv.userName] <- priv.msg

		case clientStructure := <-entering:
			if len(clients) == 0 {
				clientStructure.cliente <- pre_line + "Congrats, you were the first user."
				clientStructure.cliente <- pre_line + "You're the new IRC Server ADMIN"
				fmt.Printf("[%s] was promoted as the channel ADMIN\n", fullUser)
				admin = clientStructure.cliente
			}
			clients[clientStructure.cliente] = true
			users[fullUser] = clientStructure.cliente
			connections[fullUser] = clientStructure.connection
			connections_history[fullUser] = clientStructure.connectedSince

		case cli := <-leaving:
			if admin == cli {

				for newAdmin := range clients {
					admin = newAdmin
					newAdmin <- pre_line + "You're the new admin!"
					continue
				}
			}

			delete(clients, cli)
			close(cli)
		case user := <-kick:
			connection := connections[user]
			client := users[user]
			delete(clients, client)
			delete(connections, user)
			delete(users, user)
			delete(connections_history, user)
			connection.Close()
		}
	}
}

func handleConn(conn net.Conn) {
	//GET THE USER NAME
	var buf = make([]byte, 80)
	conn.Read(buf)
	localUser := string(bytes.Trim(buf, "\x00"))
	fullUser = localUser

	ch := make(chan string) // outgoing client messages

	// repeated usr
	if users[localUser] != nil {
		fmt.Fprintln(conn, "User with name: "+localUser+" already exists")
		close(ch)
		conn.Close()
		return
	}
	fmt.Printf("%sNew connected user [%s]\n", pre_line, localUser)
	go clientWriter(conn, ch)
	// Intro
	ch <- pre_line + "Welcome to the Simple IRC Server"
	ch <- pre_line + "Your user [" + localUser + "] is successfully logged"

	messages <- pre_line + "New connected user [" + localUser + "]"
	entering <- clientStruct{ch, conn, time.Now().Format("15:04:05\n")}

	input := bufio.NewScanner(conn)
	for input.Scan() {
		if len(input.Text()) > 0 && string(input.Text()[0]) == "/" {
			slice := strings.Split(input.Text(), " ")
			command := slice[0]
			switch command {
			// Swith Commands
			case "/msg":
				if len(slice) < 2 {
					ch <- "Specify a user"
					continue
				}
				if len(slice) < 3 {
					ch <- "Enter a message"
					continue
				}
				addressee := slice[1]
				if _, membership_test := users[addressee]; membership_test {
					directMessage := input.Text()[strings.Index(input.Text(), addressee)+len(addressee)+1:]
					direct <- userStruct{addressee, localUser + " > " + directMessage}
				} else {
					ch <- "User: " + addressee + " doesn't exist"
				}

			case "/time":
				loc, _ := time.LoadLocation("America/Mexico_City")
				theTime := time.Now().In(loc).Format("15:04\n")
				ch <- pre_line + "Local Time: " + "America/Mexico_City" + " " + strings.Trim(theTime, "\n")

			case "/user":
				if len(slice) < 2 {
					ch <- "Enter a user"
					continue
				}
				user := slice[1]
				if _, membership_test := users[user]; membership_test {
					ip := connections[user].RemoteAddr().String()
					userConnectedSince := connections_history[user]
					str := pre_line + "username: " + user + ", IP: " + ip + " - connected since " + userConnectedSince
					str = strings.TrimSuffix(str, "\n")
					ch <- str

				} else {
					ch <- "User: " + user + " doesn't exist"
				}

			case "/users":
				str := ""
				for usuario := range users {
					userConnectedSince := connections_history[usuario]
					str = str + pre_line + usuario + " - connected since " + userConnectedSince
				}
				str = strings.TrimSuffix(str, "\n")
				ch <- str

			case "/kick":
				if len(slice) < 2 {
					ch <- "Specify a user to kick"
					continue
				}
				if ch == admin {
					user := slice[1]
					if _, membership_test := users[user]; membership_test {
						messages <- pre_line + "[" + user + "] was kicked from channel" //users
						fmt.Printf("%s[%s] was kicked from channel\n", pre_line, user)  //server
						direct <- userStruct{user, pre_line + "You're kicked from this channel"}
						direct <- userStruct{user, pre_line + "For policy violation"}
						kick <- user
						kick_bool = true
					} else {
						ch <- "User: " + user + " doesn't exist"
					}
				} else {
					ch <- "Only the admin can kick people out of the server"
				}
			default:
				ch <- "Enter a valid command"
			}
		} else {
			messages <- localUser + " > " + input.Text()
		}
	}
	// When user left chat
	if !kick_bool {
		messages <- pre_line + "[" + localUser + "] left channel"
	}
	leaving <- ch
	fmt.Printf("%s[%s] left\n", pre_line, localUser)
	delete(users, localUser)
	delete(connections, localUser)
	conn.Close()
	kick_bool = false
}

func clientWriter(conn net.Conn, ch <-chan string) {
	for msg := range ch {
		fmt.Fprintln(conn, msg) // NOTE: ignoring network errors
	}
}

func main() {
	host := flag.String("host", "localhost", "localhost")
	port := flag.String("port", "9000", "the port")
	flag.Parse()
	listener, err := net.Listen("tcp", *host+":"+*port)
	fmt.Println(pre_line + "Simple IRC Server started at " + *host + ":" + *port)
	if err != nil {
		log.Fatal(err)
	}
	go broadcaster()
	fmt.Println(pre_line + "Ready for receiving new clients")
	for {
		conn, err := listener.Accept()
		if err != nil {
			log.Print(err)
			continue
		}
		go handleConn(conn)
	}
}
