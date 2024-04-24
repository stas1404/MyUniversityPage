Реализована функция `Size(ctx context.Context, d Dir) (Result, error)` в [dir_sizer.go](https://github.com/stas1404/MyUniversityPage/blob/master/Golang/directory_sizer/dir_sizer.go)

Объектное хранилище реализовано в файле [storage.go](https://github.com/stas1404/MyUniversityPage/blob/master/Golang/directory_sizer/storage.go) 

```go
// File represent a file object
type File interface {
	// Name return a fully qualified file name
	Name() string
	// Stat returns a size of file or error
	Stat(ctx context.Context) (int64, error)
}

// Dir represent a dir object
type Dir interface {
	// Name return a fully qualified dir name
	Name() string
	// Ls return a set of Dir and a set of File or error if happened
	Ls(ctx context.Context) ([]Dir, []File, error)
}
```
