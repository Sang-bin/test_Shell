int redirect_in(char ** myArgv) {
  int i;
  int fd;

  /* search forward for < */
  for (i = 0; myArgv[i] != NULL; i++) {
    if (!strcmp(myArgv[i], "<")) {
      break;
    }
  }

  if (myArgv[i]) {      /* found "<" in vector. */

    if (!myArgv[i+1]) { /* File for redirection has not been provided*/
      errno = EINVAL;
      return -1;

    } else {            /* Open file. */
      if ((fd =open(myArgv[i+1],O_RDONLY)) == -1) {
        perror(myArgv[i+1]);
        return -1;
      }
    }

    dup2(fd,STDIN_FILENO);      /* Redirect to use it for input. */
    close(fd);

    for (i = i + 2; myArgv[i] != NULL; i++) { /* close up the vector */
      myArgv[i-2] = myArgv[i];
    }
    myArgv[i-2] = NULL;      /* NULL pointer at end */
  }
  return 0;
}