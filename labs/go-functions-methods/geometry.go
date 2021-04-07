// Copyright © 2016 Alan A. A. Donovan & Brian W. Kernighan.
// License: https://creativecommons.org/licenses/by-nc-sa/4.0/

// See page 156.

// Package geometry defines simple types for plane geometry.
//!+point
package main

import (
	"fmt"
	"math"
	"math/rand"
	"os"
	"strconv"
)

func poligono(nPoints int) Path {
	var path Path

	for i := 0; i < nPoints; i++ {
		path = append(path, newPoint())
		if i == 2 {
			for pos(path[0], path[1], path[2]) == 0 {
				path[2] = newPoint()
			}
		} else if i > 2 {
			for poligonoValidate(path) == false {
				path[i] = newPoint()
			}
		}

	}
	return path
}

func pos(p, q, r Point) int {
	val := (q.getY()-p.getY())*(r.getX()-q.getX()) -
		(q.getX()-p.getX())*(r.getY()-q.getY())

	if val == 0 {
		return 0
	}
	if val > 0 {
		return 1
	}
	return 2
}

type Point struct{ X, Y float64 }

func newPoint() Point {
	newPoint := Point{}
	newPoint.setX(rand.Float64()*(101+101) - 101)
	newPoint.setY(rand.Float64()*(101+101) - 101)
	return newPoint
}
func (p *Point) setX(x float64) {
	p.X = x
}
func (p *Point) setY(y float64) {
	p.Y = y
}
func (p *Point) getX() float64 {
	return p.X
}
func (p *Point) getY() float64 {
	return p.Y
}

func seg(p, q, r Point) bool {
	if q.getX() <= math.Max(p.getX(),
		r.getX()) && q.getX() >= math.Min(p.getX(),
		r.getX()) && q.getY() <= math.Max(p.getY(),
		r.getY()) && q.getY() >= math.Min(p.getY(),
		r.getY()) {
		return true
	}
	return false
}

func intersection(p1, q1, p2, q2 Point) bool {

	o1 := pos(p1, q1, p2)
	o2 := pos(p1, q1, q2)
	o3 := pos(p2, q2, p1)
	o4 := pos(p2, q2, q1)
	// Caso principal
	if o1 != o2 && o3 != o4 {
		return true
	}

	if o1 == 0 && seg(p1, p2, q1) {
		return true
	}

	if o2 == 0 && seg(p1, q2, q1) {
		return true
	}

	if o3 == 0 && seg(p2, p1, q2) {
		return true
	}

	if o4 == 0 && seg(p2, q1, q2) {
		return true
	}
	return false
}

func distancia(p, q Point) float64 {
	return math.Hypot(q.getX()-p.getX(), q.getY()-p.getY())
}

func (p Point) distancia(q Point) float64 {
	return math.Hypot(q.getX()-p.getX(), q.getY()-p.getY())
}

//!-point

//!+path

type Path []Point

func (path Path) distancia() float64 {
	sum := 0.0
	for i := range path {
		if i > 0 {
			sum += path[i-1].distancia(path[i])
		}
	}
	sum += path[0].distancia(path[len(path)-1])
	return sum
}

func poligonoValidate(path Path) bool {
	for i := 0; i < len(path); i++ {
		if intersection(path[i%len(path)], path[(i+1)%len(path)],
			path[(i+2)%len(path)], path[(i+3)%len(path)]) == true {
			return false
		}
	}
	for i := 1; i < len(path)-3; i++ {
		if intersection(path[len(path)-1], path[0],
			path[(i)], path[(i+1)]) == true {
			return false
		}
	}
	for i := 0; i < len(path)-4; i++ {
		if intersection(path[len(path)-1], path[len(path)-2],
			path[(i)], path[(i+1)]) == true {
			return false
		}
	}
	return true
}

func main() {
	args := os.Args[1:]
	if len(args) < 1 || len(args) > 1 {
		fmt.Println("Numero incorrecto de parametros")
		return
	}
	vertex, err := strconv.Atoi(args[0])
	if err != nil {
		fmt.Println("Ns se puede convertir de string a int")
		return
	} else if vertex < 3 {
		fmt.Println("Se necesitan por lo menos 3 puntos")
		return
	}
	fmt.Println("Vertices")
	poligono := poligono(vertex)

	for i := 0; i < len(poligono); i++ {
		fmt.Printf("\t(%f,\t%f)\n", poligono[i].getX(), poligono[i].getY())
	}
	fmt.Println("Perimeter")
	for i := range poligono {
		if i > 0 && i+1 < len(poligono) {
			fmt.Printf("+%f", poligono[i].distancia(poligono[i+1]))
		} else if i == 0 {
			fmt.Printf("%f", poligono[i].distancia(poligono[i+1]))
		}
	}
	fmt.Printf("+%f\n = %f\n", poligono[0].distancia(poligono[len(poligono)-1]), poligono.distancia())
}

//!-path
