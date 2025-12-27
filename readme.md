# Desktop Pet

This is a basic project that I will use to learn more about how to implement things in C from the basic libraries they provide, or at least try to. I will try to add more and more platforms to work on, morfing the "game" to something interesting.


```bash
podman build -t c-dev-container -f container_file .
podman run -d --name c-dev-box -p 2222:2222 -v "$(pwd):/project:Z" c-dev-container
```