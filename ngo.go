package main

import (
	"C"
	"strings"
)

//export ToLower
func ToLower(payload *C.char) *C.char {
	str := strings.ToLower(C.GoString(payload))
	return C.CString(str)
}

func main() {}
