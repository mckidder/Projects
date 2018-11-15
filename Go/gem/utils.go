package main

// errorString struct
// Used for implementation of an error message
type errorString struct {
	s string
}

// New (string) error
// Returns an error that formats as the given text
func New(text string) error {
	return &errorString{text}
}

// Error() string
// Used to return a new formated error
func (e *errorString) Error() string {
	return e.s
}
