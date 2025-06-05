# WILLDO
(software by genious inventions (copyright, trademark and total ownershit do not steal))

## Compiling
You will need:
1. Meson
2. Ninja
3. PostGres
4. Libpqxx installed with homebrew

Clone the git repository. In the `main.cpp` file change the database connections to yours. You might also need to change the `libpqxx` library directories and you do not need the include dir just the dep it is some mac thing. Then run the following commands.
``` bash
meson setup build
```
Then once that is done
``` bash
meson compile -C ./build
```
Now in the `build` directory there should be a file named `WILLDO` which you can now run and it has the list and things
