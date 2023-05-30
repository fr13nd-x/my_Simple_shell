#include <stdio.h>
#include <unistd.h>


int main(void)
{

	pid_t parentpid;

	parentpid  = getppid();

	printf("%u\n", parentpid);


	return (0);


}
