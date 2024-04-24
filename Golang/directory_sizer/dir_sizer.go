package storage

import (
	"context"
	"runtime"
)

// Result represents the Size function result
type Result struct {
	// Total Size of File objects
	Size int64
	// Count is a count of File objects processed
	Count int64
}

type DirSizer interface {
	// Size calculate a size of given Dir, receive a ctx and the root Dir instance
	// will return Result or error if happened
	Size(ctx context.Context, d Dir) (Result, error)
}

// sizer implement the DirSizer interface
type sizer struct {
	// maxWorkersCount number of workers for asynchronous run
	maxWorkersCount int
}

// NewSizer returns new DirSizer instance
func NewSizer() DirSizer {
	return &sizer{}
}

func AsynchronousFileSize(ctx context.Context, f File, res_ch chan int64, err_ch chan error) {
	size, err := f.Stat(ctx)
	if err != nil {
		err_ch <- err
		return
	}
	res_ch <- size
}

func AsynchronousSize(ctx context.Context, d Dir, res_ch chan Result, err_ch chan error) {
	subdirs, files, err := d.Ls(ctx)
	if err != nil {
		err_ch <- err
		return
	}
	subdir_results := make(chan Result, len(subdirs))
	for _, subdir := range subdirs {
		// Return if context was canceled
		if ctx_err := ctx.Err(); ctx_err != nil {
			return
		}
		// Repeat recursively for all subdirectories
		go AsynchronousSize(ctx, subdir, subdir_results, err_ch)
	}
	file_size := make(chan int64, len(files))
	for _, file := range files {
		// Return if context was canceled
		if ctx_err := ctx.Err(); ctx_err != nil {
			return
		}
		go AsynchronousFileSize(ctx, file, file_size, err_ch)
	}
	res := Result{Size: 0, Count: int64(len(files))}
	var unready_amount int = len(subdirs) + len(files)
	for unready_amount != 0 {
		select {
		// Return if context was canceled
		case <-ctx.Done():
			return
		case sub_res := <-subdir_results:
			res.Size += sub_res.Size
			res.Count += sub_res.Count
			unready_amount--
		case size := <-file_size:
			res.Size += size
			unready_amount--
		}
	}
	res_ch <- res
}

func (a *sizer) Size(ctx context.Context, d Dir) (Result, error) {
	runtime.GOMAXPROCS(a.maxWorkersCount)
	// Channel to which goroutine working with d will write results
	res_ch := make(chan Result, 1)
	// Channel to which all goroutines will write errors
	err_ch := make(chan error, 1)
	cancel_ctx, cancel := context.WithCancel(ctx)
	go AsynchronousSize(cancel_ctx, d, res_ch, err_ch)
	var r Result
	select {
	// Cancel if error occurred
	case err := <-err_ch:
		cancel()
		return Result{Size: 0, Count: 0}, err
	case r = <-res_ch:
		return r, nil
	}
}
