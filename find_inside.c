#include <dirent.h>
#include <errno.h>
#include <stdio.h>
#include <string.h>
#include <stdio.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
char *Pattern;

/*Prototipo de funciones usadas*/
int isFile(char *path);
int isDirectory(char *path);
void movDir(char *path);
int BruteForce(int fd);

int main(int argc, char *argv[])
{
  int fd;
  if (argc != 3)
  {
    fprintf(stderr,"Use %s <Directorio>|<Archivo> <Patron> \n",argv[0]);
    return -1;
  }
  Pattern = argv[2];
  if (isDirectory(argv[1]))
    movDir(argv[1]);
  else
    if (isFile(argv[1]))
      if ((fd = open(argv[1],O_RDONLY)) == -1)
         fprintf(stderr,"Error al abrir el archivo %s",argv[1]);
      else
	     if(BruteForce(fd) == 0)
			printf("%s\n",argv[1]);
	else
	    {
			fprintf(stderr,"%s no es ni archivo ni directorio");
			return -1;
		}
  return 0;
}

/*Funcion para moverse dentro de los directorios*/
void movDir(char *path)
{
   struct dirent *dp;
   DIR *dir;
   char dirPATH[1024];
   int fd;
   if ((dir = opendir(path)) == NULL)
      fprintf(stderr,"Error al abrir el directorio %s \n",path);
   else
   {
	while((dp = readdir(dir)) != NULL)
	{
	  if ((strcmp(dp->d_name,".") == 0) || (strcmp(dp->d_name,"..") == 0))
	    continue;
          sprintf(dirPATH,"%s/%s",path,dp->d_name);
	  if (isDirectory(dirPATH))
             movDir(dirPATH);
	  else
            if (isFile(dirPATH))
                 if((fd = open(dirPATH,O_RDONLY)) == -1) 
                   fprintf(stderr,"No se puede abrir el archivo %s\n",dirPATH);
                 else
                   if (BruteForce(fd) == 0){
			printf("FILE %s \n",dirPATH);
                   }
	}
	while((closedir(dir) == -1) && (errno == EINTR));
   }
}

/*Prgunta si lo pasodo por parametro es un archivo*/
int isFile(char *path)
{
  struct stat statbuf;
  if (stat(path,&statbuf) == -1)
  {
    fprintf(stderr,"No se puede acceder al archivo %s \n",path);
    return 0;
  }
  else
    return S_ISREG(statbuf.st_mode);
}

/*Pregunta si lo pasado por parametro es un directorio*/
int isDirectory(char *path)
{
  struct stat statbuf;
  if (stat(path, &statbuf) == -1)
  {
    fprintf(stderr,"No se puede acceder %s\n",path);
    return 0;
  }
  else
    return S_ISDIR(statbuf.st_mode);
}

/*Algoritmo ForceBrutal para buscar un patron en un archivo*/
int BruteForce(int fd)
{
  char c;
  int j = 0;
  int n = strlen(Pattern) - 1;
  int numRead = 1;
  printf("%d %s \n",n,Pattern);
  while(numRead)
  {
    if((numRead = read(fd,&c,1)) == -1)
    {
        fprintf(stderr,"Error de lectura");
        return 1;
    }
    else
    if (numRead == 1)
    {
        if (*(Pattern + j) == c)
          j++;
        else
          j = 0;
        if (j == n)
        {
          close(fd);
	  return 0;
  	}
     }
  }
  close(fd);
  return 1;
}
