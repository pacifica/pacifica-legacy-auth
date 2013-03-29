package main

import "fmt"
import "pacifica/auth"

func main() {
	auth := auth.NewAuth(nil)
	for key, value := range auth.Services {
		fmt.Println("Service: " + key + "=" + value)
	}
	for key, value := range auth.Settings {
		fmt.Println("Setting: " + key + "=" + value)
	}
	client := auth.NewClient()
	fmt.Println("Calling testauth.")
	r, err := client.Get(auth.Services["testauth"])
	if(err != nil) {
		fmt.Println(err)
    		return
	}
	fmt.Println("Testauth done.")
	fmt.Println(r.StatusCode)
	fmt.Println("Calling logout.")
	r, err = client.Get(auth.Services["logout"])
	if(err != nil) {
		fmt.Println(err)
    		return
	}
	fmt.Println("Logout done.")
	fmt.Println(r.StatusCode)
}
