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
  private:
    CharacterStreamSingleton(CharacterStreamSingleton const&); // Don't implement
    void operator=(CharacterStreamSingleton const&); // Don't implement

    int p[2];
    FILE *read, *write;
    pthread_mutex_t lock;
};

#endif
