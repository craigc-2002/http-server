# Basic HTTP Server
Very basic minimalist HTTP server. Based on [this video](https://www.youtube.com/watch?v=2HrYIl6GpYg) by Nir Lichtman.

It has no security or error checking, so don't use it.

## To Do
- Limit files that can be served
- Send 404 for files that don't exists (current implementation segfaults)
- Add header to HTTP response (content-length needed for browser)
- Make function to handle building response
