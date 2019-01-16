Node? Go? Nopher!
---

Nopher allows NodeJS to call Golang function by using C shared library.

To enable this, the Go package needs to be build as C shared library. And a special `REGISTER` function needs to be exported to the C shared library.

```Go
//REGISTER function registers functions in the go module
//export REGISTER
func REGISTER() *C.char {
	return C.CString(strings.Join([]string{
		"ToUpper",
		"ToLower",
	}, "|"))
}
```

The `REGISTER` function should return a `*C.char` value. Functions that are available to the NodeJS environment need to be registered with the function name, separated by `|`.

And functions need to be exported by Golang as well, for example:

```Go
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
```

Then compile Go package into C shared library.

```Go
go build -o example/example.so -buildmode=c-shared example/example.go
```

Nopher will expose three APIs to 
```Bash
node example/example.js
```
