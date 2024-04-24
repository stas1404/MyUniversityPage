package main

import (
	"bytes"
	"flag"
	"fmt"
	"io"
	"os"
	"strings"
	"unicode"
	"unicode/utf8"
)

type Options struct {
	From       string
	To         string
	Offset     int64
	Limit      int64
	BlockSize  int64
	UpperCase  bool
	LowerCase  bool
	TrimSpaces bool
}

type ConversionError string

func (ConversionError ConversionError) Error() string {
	return string(ConversionError)
}

var conv_separator string = "      "

func ParseFlags() (*Options, error) {
	var opts Options
	var Conv string
	flag.StringVar(&opts.From, "from", os.Stdin.Name(), "file to read. by default - stdin")
	flag.StringVar(&opts.To, "to", os.Stdout.Name(), "file to write. by default - stdout")
	flag.Int64Var(&opts.Offset, "offset", 0, "how much should been skipped")
	flag.Int64Var(&opts.Limit, "limit", -1, "maximal amount of bytes")
	flag.Int64Var(&opts.BlockSize, "block-size", 2, "maximal amount of bytes read at one step")
	flag.StringVar(&Conv, "conv", "", "conversations with a file\n   choose any of\n"+
		conv_separator+
		"* upper_case - to get all letters in file written in upper_case \n"+
		conv_separator+
		"* lower_case - to "+
		"get all letters in file written in lower_case \n"+
		conv_separator+
		"* trim_spaces - to skip space symbols at very beginning and at the end of file")
	flag.Parse()
	conversions := strings.Split(Conv, ",")
	for _, conv := range conversions {
		switch conv {
		case "upper_case":
			opts.UpperCase = true
		case "lower_case":
			opts.LowerCase = true
		case "trim_spaces":
			opts.TrimSpaces = true
		default:
			if conv != "" {
				fmt.Fprintln(os.Stderr, "cat not parse flag --conv ", conv,
					": not in [upper_case, lower_case, trim_spaces]")
				os.Exit(1)
			}
		}
	}
	if opts.UpperCase && opts.LowerCase {
		fmt.Fprintln(os.Stderr, "can not converse file to upper and lower cases together")
		os.Exit(1)
	}
	return &opts, nil
}

func ReadOffset(Reader *io.Reader, bytes []byte, opts *Options) {
	var window int64
	window = int64(len(bytes))
	for opts.Offset > window {
		_, err := (*Reader).Read(bytes)
		for err != nil {
			fmt.Fprintln(os.Stderr, "can not skip ", opts.Limit, "bytes:", err)
			os.Exit(1)
		}
		opts.Offset -= window
	}
	if opts.Offset != 0 {
		optional_bytes := make([]byte, opts.Offset)
		_, err := (*Reader).Read(optional_bytes)
		if err != nil {
			fmt.Fprintln(os.Stderr, "can not skip ", opts.Limit, "bytes:", err)
			os.Exit(2)
		}
	}
}

func SkipSpaces(Reader *io.Reader) (byte, error) {
	bytes := make([]byte, 1)
	_, err := (*Reader).Read(bytes)
	if err != nil && err != io.EOF {
		fmt.Fprintln(os.Stderr, "can not skip spaces:", err)
		os.Exit(2)
	}
	rune, _ := utf8.DecodeRune(bytes)
	for unicode.IsSpace(rune) && err == nil {
		_, err = (*Reader).Read(bytes)
		rune, _ = utf8.DecodeRune(bytes)
	}
	if err != nil {
		return 0, err
	}
	return bytes[0], nil
}

func Write(writer *io.Writer, options *Options, rune_candidate []byte, spaces *[]byte) error {
	if options.UpperCase {
		tmp := bytes.ToUpper(rune_candidate)
		if rune, _ := utf8.DecodeRune(tmp); rune != utf8.RuneError && utf8.ValidRune(rune) {
			rune_candidate = tmp
		}
	}
	if options.LowerCase {
		tmp := bytes.ToLower(rune_candidate)
		if rune, _ := utf8.DecodeRune(tmp); rune != utf8.RuneError && utf8.ValidRune(rune) {
			rune_candidate = tmp
		}
	}
	if rune, _ := utf8.DecodeRune(rune_candidate); options.TrimSpaces && unicode.IsSpace(rune) {
		for _, b := range rune_candidate {
			*spaces = append(*spaces, b)
		}
		return nil
	}
	_, err := (*writer).Write(*spaces)
	if err != nil {
		return err
	}
	*spaces = nil
	_, err = (*writer).Write(rune_candidate)
	if err != nil {
		return err
	}
	return nil
}

func WriteTogether(Writer *io.Writer, possible_rune []byte, options *Options, spaces *[]byte) bool {
	rune, _ := utf8.DecodeRune(possible_rune)
	if rune != utf8.RuneError && utf8.ValidRune(rune) {
		err := Write(Writer, options, possible_rune, spaces)
		if err != nil {
			_, _ = fmt.Fprintln(os.Stderr, "can not write to", options.To, " : ", err)
			os.Exit(1)
		}
		return true
	}
	return false
}

func WriteByAllWays(Writer *io.Writer, possible_rune []byte, options *Options, spaces *[]byte) {
	err := Write(Writer, options, possible_rune, spaces)
	if err != nil {
		_, _ = fmt.Fprintln(os.Stderr, "can not write to", options.To, " : ", err)
		os.Exit(1)
	}
}

func main() {
	opts, err := ParseFlags()
	if err != nil {
		fmt.Fprintln(os.Stderr, "can not parse flags:", err)
		os.Exit(1)
	}
	From, err := os.Open(opts.From)
	if err != nil {
		_, _ = fmt.Fprintln(os.Stderr, "can not open", opts.From, " : ", err)
		os.Exit(1)
	}
	defer From.Close()
	if _, err := os.Stat(opts.To); err == nil && opts.To != os.Stdout.Name() {
		fmt.Fprintln(os.Stderr, "writing file already exists")
		os.Exit(1)
	}
	To, err := os.Create(opts.To)
	if err != nil {
		_, _ = fmt.Fprintln(os.Stderr, "can not open", opts.From, " : ", err)
		os.Exit(1)
	}
	defer To.Close()
	Reader := io.Reader(From)
	Writer := io.Writer(To)
	rune_candidate := make([]byte, 1)
	ReadOffset(&Reader, rune_candidate, opts)
	if opts.Limit != -1 {
		Reader = io.LimitReader(Reader, opts.Limit)
	}
	var unwrited byte
	byte_was_unwrited := false
	if opts.TrimSpaces {
		unwrited, err = SkipSpaces(&Reader)
		if err != nil && err != io.EOF {
			fmt.Fprintln(os.Stderr, "can not skip spaces :", err)
		}
		if err == io.EOF {
			return
		}
		byte_was_unwrited = true
	}
	var errwr error
	_, err = Reader.Read(rune_candidate)
	spaces := make([]byte, 0)
	for err == nil && errwr == nil {
		if byte_was_unwrited {
			succes := WriteTogether(&Writer, []byte{unwrited}, opts, &spaces)
			if succes {
				unwrited = 0
				byte_was_unwrited = false
				continue
			}
			succes = WriteTogether(&Writer, []byte{unwrited, rune_candidate[0]}, opts, &spaces)
			byte_was_unwrited = false
			if succes {
				_, err = Reader.Read(rune_candidate)
				continue
			}
			if !succes {
				WriteByAllWays(&Writer, []byte{unwrited}, opts, &spaces)
			}
		}
		succes := WriteTogether(&Writer, rune_candidate, opts, &spaces)
		if !succes {
			unwrited = rune_candidate[0]
			byte_was_unwrited = true
		}
		_, err = Reader.Read(rune_candidate)
	}
	if byte_was_unwrited {
		WriteByAllWays(&Writer, []byte{unwrited}, opts, &spaces)
	}
}
