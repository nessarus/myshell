char *strtemp = strchr(t->argv[0],'/');
if(strtemp!=NULL)
{
    exit(execv(t->argv[0],t->argv));
}

find_path_execute(t);
exit(EXIT_FAILURE);
