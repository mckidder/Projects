package main

import (
	"crypto/aes"
	"crypto/cipher"
	"crypto/md5"
	"crypto/rand"
	"encoding/base64"
	"encoding/hex"
	"fmt"
	"io"
)

type (
	file struct {
		CRC32Q      string
		initialPath string
		finalPath   string
	}
)

func test() {
	/*
		fmt.Print("Enter File Path: ")
		reader := bufio.NewReader(os.Stdin)
		inputFile, _ := reader.ReadString('\n')

		// Load file []byte into memory
		encryptedFile, err := ioutil.ReadFile(inputFile)
		if err != nil {
			panic(err)
		}

		// crc32q := crc32.MakeTable(0xD5828281)
		// c1 := crc32.Checksum(d1, crc32q)

		ID := sid.Id()
		key := []byte(createHash(ID))
		ed := encryptAESFile(encryptedFile, key)

		// Create encrypted file
		err = ioutil.WriteFile(inputFile+"_enc", ed, 0644)
		if err != nil {
			panic(err)
		}

		// Decrypt []byte data with key
		decryptedData := decryptAESFile(d2, key)

			// Create encrypted file
			err = ioutil.WriteFile("/home/owner/Code/gfe/img1.png", decryptedData, 0644)
			if err != nil {
				panic(err)
			}

			// Create encrypted file
			err = ioutil.WriteFile("/home/owner/Code/gfe/output2.jpg", decryptedData, 0644)
			if err != nil {
				panic(err)
			}
	*/
}

// createHash(string)
// Creates an hashed string from a key
func createHash(key string) string {
	hasher := md5.New()
	hasher.Write([]byte(key))
	return hex.EncodeToString(hasher.Sum(nil))
}

// encryptAESMessage([]byte, string)
// Encrypts a 64bit string
func encryptAESMessage(key []byte, text string) string {
	pt := []byte(text)

	block, err := aes.NewCipher(key)
	if err != nil {
		panic(err)
	}

	ct := make([]byte, aes.BlockSize+len(pt))
	iv := ct[:aes.BlockSize]
	if _, err := io.ReadFull(rand.Reader, iv); err != nil {
		panic(err)
	}

	stream := cipher.NewCFBEncrypter(block, iv)
	stream.XORKeyStream(ct[aes.BlockSize:], pt)

	// convert to base64
	return base64.URLEncoding.EncodeToString(ct)
}

// decryptAESMessage([]byte, string)
// Decrpyts an encrypted 64bit string
func decryptAESMessage(key []byte, cryptoText string) string {
	ct, _ := base64.URLEncoding.DecodeString(cryptoText)

	block, err := aes.NewCipher(key)
	if err != nil {
		panic(err)
	}

	if len(ct) < aes.BlockSize {
		panic("ciphertext too short")
	}
	iv := ct[:aes.BlockSize]
	ct = ct[aes.BlockSize:]

	stream := cipher.NewCFBDecrypter(block, iv)

	stream.XORKeyStream(ct, ct)

	return fmt.Sprintf("%s", ct)
}

// encryptAESFile([]byte, []byte)
// Encrypts a 64bit file
func encryptAESFile(code, key []byte) []byte {
	block, err := aes.NewCipher(key)
	if err != nil {
		panic(err)
	}

	ct := make([]byte, aes.BlockSize+len(code))
	iv := ct[:aes.BlockSize]
	if _, err := io.ReadFull(rand.Reader, iv); err != nil {
		panic(err)
	}

	stream := cipher.NewCFBEncrypter(block, iv)
	stream.XORKeyStream(ct[aes.BlockSize:], code)

	// convert to base64
	return ct
}

// decdecryptAESFile([]byte, []byte)
// Decrpyts an encrypted 64bit file
func decryptAESFile(code, key []byte) []byte {
	block, err := aes.NewCipher(key)
	if err != nil {
		panic(err)
	}

	if len(code) < aes.BlockSize {
		panic("ciphertext too short")
	}
	iv := code[:aes.BlockSize]
	code = code[aes.BlockSize:]

	stream := cipher.NewCFBDecrypter(block, iv)

	stream.XORKeyStream(code, code)

	return code
}
