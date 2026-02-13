# :globe_with_meridians: WEB_SERV

---

The **Webserv** project aims to create a functional **HTTP server** from scratch using **C++**. 
Inspired by the architecture of *Nginx*, this server is capable of handling multiple websites, managing client requests, and executing CGI scripts, all while using a non-blocking I/O multiplexing system.


**Contributors**:

| Avatar | Name | GitHub |
| :---: | :--- | :--- |
| <img src="https://github.com/Meruem-42.png" width="40"> | *Meruem* |[@meruem](https://github.com/Meruem-42)
| <img src="https://github.com/l-devigne.png" width="40"> | *ldevigne* |[@ldevigne](https://github.com/l-devigne)
| <img src="https://github.com/lflayeux.png" width="40"> | *lflayeux* |[@lflayeux](https://github.com/lflayeux)


---

## :hammer_and_wrench: Installation & Usage


### 1. Clone the Project

First, **clone** the repository to your local machine:

``` bash
git clone git@github.com:lflayeux/web_server.git <directory_name>
```

### 2. Customize your server 

You can **customize** your server by **modifying the .conf** file. Detailed instructions can be found in our [*Official .conf Documentation*](#notebook_with_decorative_cover-official-conf-documentation)

### 3. Launch the program

You can now **launch** your own server:

```bash
./webserv <path_to_your_conf>
```

### 4. Test Your Server

Once the server is running, **open** your  web browser and **navigate** to:

```http
http://localhost:<port>  
```
>  

> :warning: **Recommended Browser**: We strongly recommend using **Mozilla Firefox** for testing. All our stability and performance tests were conducted on it, ensuring the most reliable experience.

---

## :notebook_with_decorative_cover: Official .conf documentation 

The **`.conf`** file is the heart of the project. It allows us to configure the web server to handle **one or multiple sites**, either on the same port or on different ones for each. It also defines how the website reacts based on the path requested by the client. The logic and functionality are primarily based on [_nginx_](https://nginx.org/en/docs/) configuration files.

### 1. Server directives

In the configuration file, **each website** is defined by a `server` block. This block acts as a container for all the settings related to a specific host.

```nginx
server {
	# Directives go here
	listen 8080;
	root /;
}
```

#### Mandatory directives

- `listen` Defines the port(s) on which the server will listen for this specific website. You can assign multiple ports to the same server block.
	```nginx
	server {
		listen 8080;
		# Syntax: listen <port>;
	}
	```
> :warning: **Warning**: If not defined your .conf is invalid.  

- `root` Specifies the base directory used to search for files. While technically optional if every path is covered by a `location`, it is strongly recommended to define a global `root` as a fallback.
	```nginx
	server {
		root /;
		# Syntax: root <path_to_directory>;
	}
	```

#### Optional directives

- `error_pages` Defines a custom HTML page to be displayed for specific HTTP error codes. This allows you to provide a more user-friendly experience than the default server error messages.
	```nginx
	server {
		error_page 404 /errors/404.html;
		error_page 500 502 /errors/server_error.html;
		# Syntax: error_page <code(s)> <path_to_html_file>;
	}
	```

- `client_max_body_size` Defines the maximum allowed size of a client's request body. This is crucial for controlling file uploads and preventing server abuse.
	```nginx
	server {
		# Syntax: client_max_body_size <number>[K|M|G];
    	client_max_body_size 1024;  # 1024 bytes
    	client_max_body_size 50K;   # 50 Kilobytes
    	client_max_body_size 10M;   # 10 Megabytes
    	client_max_body_size 1G;    # 1 Gigabyte
	}
	```
> :page_facing_up: **Note**: If a request exceeds this size, the server will return a `413 Payload Too Large` error.

- `location` block used to define specific rules and behaviors for a **precise URI** path. It allows you to customize how the server handles **different directories or files**.
Detailed documentation for the `location` block can be found in the [next section](#2-location-directives).

### 2. Location directives

A `location` block allows you to define specific rules for a precise URI path.
> :page_facing_up: **Prefix Matching**: Any request URL that starts with the specified path will inherit the rules defined within that block. For example, a block for `/upload` will catch `/upload`, `/upload/test.txt`, and `/upload/photos/`.

```nginx
server {
	location /upload {
		# Specific rules for the /upload directory go here
	}
}
```

#### Optional directives

- `root` refere to [root directives](#mandatory-directives)  in server blocks

- `client_max_body_size` [client_max_body_size directives](#mandatory-directives) in server blocks

- `allowed_method` Defines method allowed for this `location` block
	```nginx
	server {
		location {
			allowed_method GET POST DELETE;
			# Syntax: allowed_method <method(s)>;
		}
	}
	```

- `index` Defines the default file(s) to look for when a directory is requested. The server will check for these files in the order they are listed.
	```nginx
	server {
		listen 8080;
		location / {
			root ./html/;
			index index.html index.htm;
			# Syntax: index <files_in_order_of_pritority> ;
		}
	}
	```

> :page_facing_up: **Behavior**: If you request http://localhost:8080/, the server will first try to find `./html/index.html`. If it doesn't exist, it will look for `./html/index.htm`

- `autoindex` Enables or disables the directory listing feature. If a request points to a directory and no `index` file is found, the server will generate an HTML page listing all the files and folders within that directory.
	```nginx
	server {
		listen 8080;
		location / {
			root ./html/;
			index index.html index.htm;
			autoindex on;
			# Syntax: autoindex <on/off> ;
		}
	}
	```
> :page_facing_up: **Note** : By default if not defined autoindex is off

- `return` Used to redirect a client to a different URL. It immediately stops the request processing and sends a redirection status code to the browser.

	```nginx
	server {
		listen 8080;
		location / {
			root ./html/;
			# 1. Direct status code (4xx or 5xx code only)
			return 404;
			# 2. Redirection to a local path (with optional 3xx code)
			return 300 /home/;
			# 3. Redirection to an external URL
			return http::google.com;
		}
	}
	```


- `upload_allowed`A specific toggle to enable or disable the ability for clients to upload files via POST requests in this location.
	```nginx
	server {
		listen 8080;
		location / {
			root ./html/;
			upload_allowed on;
			# Syntax: upload_allowed <on/off> ;
			upload_location ./data;
		}
	}
	```
> :page_facing_up: **Note**:  By default if not defined upload_allowed is off. If turned on you need to specify an `upload_location`.

- `upload_location` Defines the specific directory where uploaded files should be saved on the server's file system.

	```nginx
	server {
		listen 8080;
		location / {
			root ./html/;
			upload_allowed on;
			upload_location ./data;
			# Syntax: upload_location <path_to_upload_location>;
		}
	}
	```

--- 

## :notebook_with_decorative_cover: Handling connections

### 1. Understanding the `socket()` function
The `socket()` function creates a network endpoint (a file descriptor) that acts as a gateway for communication. In our project, it is the foundation that allows a server to wait for and handle client requests.

The setup process follows a specific order:

- Socket Creation: We define how the socket handles communication (e.g., the IP family like IPv4 and the transport protocol, typically TCP).
- `setsockopt()`: This is used to configure options on the server's socket file descriptor. A common option is SO_REUSEADDR, which allows the server to bind to a port even if it was recently closed, preventing "Address already in use" errors during restarts.
- Addressing (sockaddr_in): We define the server's identity using structures to specify the `sin_family` (IPv4), `sin_port` (the listening port), and `sin_addr` (the interface IP).
- `bind()`: This system call associates the socket with the specific IP address and port number defined above.
- `listen()`: Finally, the socket enters a passive state, waiting for incoming connection attempts ("knocking at the door").

### 2. Understanding the `epoll()` function
Once the socket is listening, we need an efficient way to manage multiple connections. This is where `epoll` comes in. `epoll` is an I/O event notification facility. Instead of constantly checking every single connection (polling), `epoll` monitors a set of file descriptors and notifies the server when an "event" occurs (e.g., a new connection is ready to be accepted or data is ready to be read). It uses flags (like `EPOLLIN` or `EPOLLOUT`) to define how we want to be notified of these changes.

### 3. How to handle many servers simultanousely
We decided to use a `std::vector` to store all the ports defined in our .conf file. We then implemented a `create_multi_serv()` function that takes this vector and the epoll_fd as parameters. This function returns a vector of server socket file descriptors (server_socket_fds).

Finally, this vector is used within our main loop to ensure that all server sockets are actively listening and monitored by epoll for incoming connections.