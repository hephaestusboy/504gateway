#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <curl/curl.h>
#include <unistd.h> // For sleep()

// Callback function to handle the response data
size_t write_callback(void *contents, size_t size, size_t nmemb, void *userp) {
    size_t total_size = size * nmemb;
    strcat((char*)userp, (char*)contents);
    return total_size;
}

// Function to open the URL in the default web browser
int open_in_browser(const char *url) {
    #ifdef _WIN32
        // For Windows
        char command[256];
        snprintf(command, sizeof(command), "start \"\" \"%s\"", url);
        return system(command);
    #else
        // For Unix-like systems (Linux, macOS, etc.)
        char command[256];
        snprintf(command, sizeof(command), "xdg-open \"%s\" 2>/dev/null || open \"%s\"", url, url);
        return system(command);
    #endif
}

int main(void) {
    CURL *curl;
    int flag=0;
    CURLcode res;
    char response[16384]; // Buffer to store the response
    long http_code = 0;
    const char *url ="https://app.ktu.edu.in/login.htm";

    // Initialize libcurl
    curl_global_init(CURL_GLOBAL_DEFAULT);
    curl = curl_easy_init();
    curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0L);
    curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0L);
    if(curl) {
        // Set the URL of the webpage to fetch
        curl_easy_setopt(curl, CURLOPT_URL, url);
        
        // Set the write callback function to handle the response
        response[0] = '\0'; // Initialize the buffer
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_callback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, response);
        
        // Perform the request
        res = curl_easy_perform(curl);
        loop:
        // Check for errors
        if(res != CURLE_OK) {
            fprintf(stderr, "curl_easy_perform() failed: %s\n", curl_easy_strerror(res));
        } else {
            // Get the HTTP response code
            curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &http_code);
            printf("Response Code: %ld\n", http_code);
            
            // Check if the response code is 200 (OK)
            if(http_code == 200) {
                printf("Response Body:\n%s\n", response);
                // Optionally, save the response to a file
                FILE *file = fopen("response.html", "w");
                if (file) {
                    fputs(response, file);
                    fclose(file);
                    printf("Response saved to response.html\n");
                } else {
                    fprintf(stderr, "Failed to open file for writing\n");
                }
                
                // Attempt to open the URL in the default browser
                int success = 0;
                while (!success) {
                    success = (open_in_browser(url) == 0);
                    if (!success) {
                        printf("Failed to open browser. Retrying...\n");
                        sleep(1); // Wait for 1 second before retrying
                    }
                }
                printf("Successfully opened the URL in the browser.\n");
		flag=1;
            } else {
                printf("Request failed with response code %ld\n", http_code);
		flag=0;
            }
	sleep(5);
	if(flag==0){
		goto loop;
        }
	}
        
        // Clean up
        curl_easy_cleanup(curl);
    }
    
    curl_global_cleanup();
    return 0;
}
