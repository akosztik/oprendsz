#include <stdio.h>
#include <sys/types.h>

void NSO(pid_t idomar, pid_t madar)
{
	printf("[NSO] Az idomar pidje: %i\n", idomar);
	printf("[NSO] A madar pidje: %i\n", madar);
}

void trainer()
{
	printf("[Idomar] Az en pidem: %i\n", getpid());
	printf("[Idomar] A Ne-Sas-Old Kft. pidje: %i\n", getppid());
}

void eagle()
{
	printf("[Madar] Az en pidem: %i\n", getpid());
	printf("[Madar] A Ne-Sas-Old Kft. pidje: %i\n", getppid());
}

int main()
{
	pid_t idomar = fork();
	if(idomar)
	{
		pid_t madar = fork();
		if(madar)
		{
			NSO(idomar, madar);
		}
		else
		{
			eagle();
		}
	}
	else
	{
		trainer();
	}
	return 0;
}
