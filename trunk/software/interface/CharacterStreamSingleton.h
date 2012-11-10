#ifndef CHARACTERSTREAM_SINGLETON_H
#define CHARACTERSTREAM_SINGLETON_H

#include <unistd.h>
#include <pthread.h>
#include <stdio.h>

class CharacterStreamSingleton {
  public:
    CharacterStreamSingleton() : read(NULL), write(NULL)
    {
      if (pipe(p)) {
        puts("pipe failed");
        exit(1);
      }
      read = fdopen(p[0], "r");
      write = fdopen(p[1], "w");

      pthread_mutex_init(&lock, NULL);
    }

    ~CharacterStreamSingleton()
    {
       fclose(read);
       fclose(write);
       pthread_mutex_destroy(&lock);
    }

    static CharacterStreamSingleton& get_instance()
    {
      static CharacterStreamSingleton instance;
      return instance;
    }

    void write_char(char c)
    {
      pthread_mutex_lock(&lock);
      fputc(c, write);
      fflush(write);
      pthread_mutex_unlock(&lock);
    }

    FILE *read_file()
    {
      return read;
    }

    char wait_key(int milliseconds)
    {
      fd_set readfds;
      struct timeval tv = {milliseconds / 1000, 1000 * (milliseconds % 1000)};

      // May use ncurses
      FILE *cs_fd = read_file();
      int cs_fileno = 0;

      FD_ZERO(&readfds);
      FD_SET(STDIN_FILENO, &readfds);
      if (cs_fd) {
        cs_fileno = fileno(cs_fd);
        FD_SET(cs_fileno, &readfds);
      }
        
      if (select(FD_SETSIZE, &readfds, NULL, NULL, &tv) > 0) {
        if (FD_ISSET(STDIN_FILENO, &readfds)) {
          return fgetc(stdin);
        }
        if (cs_fd && FD_ISSET(cs_fileno, &readfds)) {
          return fgetc(cs_fd);
        }
      }

      return '\0';
    }
  private:
    CharacterStreamSingleton(CharacterStreamSingleton const&); // Don't implement
    void operator=(CharacterStreamSingleton const&); // Don't implement

    int p[2];
    FILE *read, *write;
    pthread_mutex_t lock;
};

#endif
