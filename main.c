#define FUSE_USE_VERSION 25

#include <fuse.h>
#include <string.h>
#include <unistd.h>
#include <time.h>
#include <errno.h>
#include <stdio.h>
#include <fcntl.h>
#include <stddef.h>
#include <string.h>
#include <malloc.h>
#include <libgen.h>

struct nodito{
    char nombre[64];
    char tipo;
    struct nodito *primer_hijo;
    struct nodito *sig;
};


struct nodito root = {"/",'D', NULL,NULL};

struct nodito  *Agregar_nodito(char *nombre, struct nodito *padre, char tipo){
    struct nodito *nodo = (struct nodito *)malloc(sizeof(struct nodito));
    strcpy(nodo->nombre,nombre);
    nodo->tipo = tipo;
    nodo->primer_hijo=NULL;
    nodo->sig=NULL;
    if(padre->primer_hijo == NULL){
        padre->primer_hijo = nodo;
    }else{
        struct nodito *p = padre->primer_hijo;
        while(p->sig != NULL){
            p = p->sig;
        }
        p->sig = nodo;
    }
    return nodo;
};

struct nodito *buscarnombre(struct nodito *p, char *nombre){
    while(p != NULL){
        printf("%s es aqui \n", p->nombre);
        if(strcmp(p->nombre,nombre) == 0){
            break;
        }
        p = p->sig;
    }
    return p;


}
struct nodito *Buscar_nodito(const char *path){
        if(strcmp(path,"/") == 0){
            return &root;
        }
        if(root.primer_hijo == NULL){
            return NULL;
        }
        struct nodito *p = root.primer_hijo;
        struct nodito *r;
        char *copia = (char *)malloc(strlen(path)+1);
        strcpy(copia,path);

        char *token = strtok(copia,"/");
        while(token!=NULL){
            r = buscarnombre(p,token);
            if(r==NULL){
                free(copia);
                return NULL;
            }
            token = strtok(NULL,"/");
            if(token != NULL){
                if(r->primer_hijo !=NULL){
                    p = r->primer_hijo;
                }else{
                    free(copia);
                    return NULL;
                }
            }
        }
        free(copia);
        return r;
    }

// obtiene atributos
static int stanleefs_getattr(const char *path, struct stat *st)
{
    printf( "En stanleefs_getattr(): [%s]\n", path );

    st->st_uid = getuid();
    st->st_gid = getgid();
    st->st_atime = time( NULL );
    st->st_mtime = time( NULL );

    struct nodito *p = Buscar_nodito(path);
    if(p == NULL){
        return -ENOENT;
    }
    if(p->tipo == 'D'){
        st->st_mode = S_IFDIR | 0755;
        st->st_nlink = 3; // http://unix.stackexchange.com/a/101536
    }
    else
    {
        st->st_mode = S_IFREG | 0644;
        st->st_nlink = 1;
        st->st_size = 0;
    }

    return 0;
}

// crea directorio
static int stanleefs_mkdir(const char *path, mode_t mode)
{
    printf( "En stanleefs_mkdir(): [%s]\n", path );
    char *str1 = strdup(path);
    char *str2 = strdup(path);
    char *ruta = dirname(str1);
    char *nombre =basename(str2);
    struct nodito *p = Buscar_nodito(ruta);
    if(p == NULL){
        return -ENOENT;
    }
    Agregar_nodito(nombre, p, 'D');
    return 0;

}

// lee directorio
static int stanleefs_readdir( const char *path, void *buffer, fuse_fill_dir_t filler, off_t offset, struct fuse_file_info *fi )
{
    printf( "En stanleefs_readdir(): [%s]\n", path );
    struct nodito *p = Buscar_nodito(path);
    if(p==NULL){
        return -ENOENT;
    }

    filler( buffer, ".", NULL, 0 ); // Current Directory
    filler( buffer, "..", NULL, 0 ); // Parent Directory
    p = p->primer_hijo;
    while(p != NULL){
        filler( buffer, p->nombre, NULL, 0 );
        p = p->sig;
    }

    return 0;
}


// crea nodo de archivo
static int stanleefs_mknod( const char *path, mode_t mode, dev_t dev)
{
    printf( "En stanleefs_mknod(): [%s]\n", path );
    char *str1 = strdup(path);
    char *str2 = strdup(path);
    char *ruta = dirname(str1);
    char *nombre =basename(str2);
    struct nodito *p = Buscar_nodito(ruta);
    if(p == NULL){
        return -ENOENT;
    }
    Agregar_nodito(nombre, p, 'F');
    return 0;

    return -EACCES;
}

// abre un archivo
static int stanleefs_open(const char *path, struct fuse_file_info *fi)
{
    printf( "En stanleefs_open(): [%s]\n", path );
    return 0;
    //return -EACCES;

}

// lee desde un archivo
static int stanleefs_read( const char *path, char *buffer, size_t size, off_t offset, struct fuse_file_info *fi )
{

    printf( "En stanleefs_read(): [%s]\n", path );
    return 0;
}


// graba en un archivo
static int stanleefs_write(const char *path, const char *buff, size_t size, off_t off, struct fuse_file_info *fi)
{
    printf( "En stanleefs_mknod(): [%s]\n", path );
    return size;
    //return -EACCES;
}

static struct fuse_operations stanleefs_opers = {
    .getattr    = stanleefs_getattr,
    .mkdir      = stanleefs_mkdir,
    .readdir    = stanleefs_readdir,
    .mknod      = stanleefs_mknod,
    .open       = stanleefs_open,
    .read       = stanleefs_read,
    .write      = stanleefs_write,

};



int main( int argc, char *argv[] )
{
    return fuse_main(argc,argv, &stanleefs_opers );
}
