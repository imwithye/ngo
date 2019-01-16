package main

import (
	"C"
	"strings"
)

//ToUpper function converts a string to upper case
//export ToUpper
func ToUpper(payload *C.char) *C.char {
	str := strings.ToUpper(C.GoString(payload))
	return C.CString(str)
}

//ToLower function converts a string to lower case
//export ToLower
func ToLower(payload *C.char) *C.char {
	str := strings.ToLower(C.GoString(payload))
	return C.CString(str)
}

//REGISTER function registers functions in the go module
//export REGISTER
func REGISTER() *C.char {
	return C.CString("ToLower|ToUpper")
}

func main() {}
