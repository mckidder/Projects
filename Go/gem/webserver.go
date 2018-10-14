package main

import (
	"fmt"
	"html/template"
	"log"
	"net/http"
	"time"

	"github.com/chilts/sid"
	"github.com/chyeh/pubip"
)

// Client contains vairables used for html page
type Client struct {
	ClientID  string
	ClientIP  string
	Text      string
	Encrypted string
	Decrypted string
}

func main() {
	http.HandleFunc("/", homePage)
	http.Handle("/static/", http.StripPrefix("/static/", http.FileServer(http.Dir("static"))))
	log.Fatal(http.ListenAndServe(":8080", nil))
}

// homePage(http.ResponseWriter, *http.Request)
// Handles loading and parsing of HTML page
func homePage(w http.ResponseWriter, r *http.Request) {

	// Parse HTML page template
	t, err := template.ParseFiles("index.html")
	if err != nil {
		log.Print(err)
	}

	// Get outward ClientIP
	ip, err := pubip.Get()
	if err != nil {
		log.Println(err)
	}

	// Generate ClientID
	ID := sid.Id()

	// Create byte data from key
	key := []byte(createHash(ID))

	// Random time for testing
	pt := fmt.Sprintf("%v", time.Now())

	// Encrpyt text with key
	en := encryptAESMessage(key, pt)

	de := decryptAESMessage([]byte(createHash("1LF_u0Xx4mX-0muP9Tk9edd")), en)

	// Execute template and with variables
	err = t.Execute(w, Client{
		ClientID:  ID,
		ClientIP:  fmt.Sprintf("%v", ip),
		Text:      pt,
		Encrypted: en,
		Decrypted: de,
	})
	if err != nil {
		log.Print(err)
	}
}
