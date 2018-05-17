
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <termios.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <signal.h>
#include <sys/ipc.h>
#include <sys/wait.h>
#include <sys/shm.h>
#include <poll.h>



#define MSG_STACK_SIZE 6

#define clrscr()			printf("\033[H\033[J")
#define gotoxy(x,y)			printf("\033[%d;%dH", (x), (y))
#define setcolor(fgc,bgc)	printf("\e[%im\e[%im", fgc, bgc)


// ========================================
//  TYPE DEFINITIONS
// ========================================

typedef struct order_data {
	time_t		date;
	char		name[30];
	char		address[45];
	char		email[35];
	char		phone[15];
	int			priority;
} t_order;
typedef struct list_item {
	t_order		data;
	int			visible;
} t_item;
typedef struct process_message {
	long		mtype;
	char		mtext[65];
} t_msg;
typedef struct work_sheet {
	t_order		order;
	int			idx;
	int			time;
} t_ws;


// ========================================
//  GLOBAL VARIABLES
// ========================================

// Terminal
struct termios	oldt, newt;
int				oldf;

// Process PIDs
pid_t p_msg, p_madar, p_idomar;

// Order list
size_t			list_count			= 0;
size_t			visible_list_count	= 0;
t_item *		list_items			= NULL;
t_order *		visible_items		= NULL;

char			filter_str[20]		= "";
int				filter_prior		= 0;

// Inter-process message queue
int				msg_queue;
t_ws			worksheet;

// Pipeline
int				pipe_main[2];
int				pipe_idomar[2];

// Message stack
int				msg_stack_size		= 0;
char			msg_stack[MSG_STACK_SIZE][64];
int				msg_stack_shared_id;
char *			msg_stack_shared;


// ========================================
//  HELPER FUNCTIONS
// ========================================

char * date_to_str(time_t timestamp)
{
	struct tm *t = localtime(&timestamp);
	static char date_string[11];

	strftime(date_string, 11, "%Y-%m-%d", t);

	return date_string;
}
char * time_to_str(time_t timestamp)
{
	struct tm *t = localtime(&timestamp);
	static char date_string[9];

	strftime(date_string, 9, "%H:%M:%S", t);

	return date_string;
}
char * stristr3(const char* haystack, const char* needle) {
	do {
		const char* h = haystack;
		const char* n = needle;
		while (tolower((unsigned char)*h) == tolower((unsigned char)*n) && *n) {
			h++;
			n++;
		}
		if (*n == 0) {
			return (char *)haystack;
		}
	} while (*haystack++);
	return 0;
}
void flush_stdin()
{
	char c;
	while ((c = getchar()) != '\n' && c != EOF);
}
void make_invoice(t_order order)
{
	char filename[] = "invoice-";
	strcat(filename, order.name);
	strcat(filename, ".txt");
	int i = 0;
	while (filename[i++])
	{
		if (isspace(filename[i]))
			filename[i] = '_';
	}

	FILE *invoice;
	invoice = fopen(filename, "w");

	fputs("\n", invoice);
	fputs("███╗   ██╗███████╗    ███████╗ █████╗ ███████╗       ██████╗ ██╗     ██████╗ \n", invoice);
	fputs("████╗  ██║██╔════╝    ██╔════╝██╔══██╗██╔════╝      ██╔═══██╗██║     ██╔══██╗\n", invoice);
	fputs("██╔██╗ ██║█████╗█████╗███████╗███████║███████╗█████╗██║   ██║██║     ██║  ██║\n", invoice);
	fputs("██║╚██╗██║██╔══╝╚════╝╚════██║██╔══██║╚════██║╚════╝██║   ██║██║     ██║  ██║\n", invoice);
	fputs("██║ ╚████║███████╗    ███████║██║  ██║███████║      ╚██████╔╝███████╗██████╔╝\n", invoice);
	fputs("╚═╝  ╚═══╝╚══════╝    ╚══════╝╚═╝  ╚═╝╚══════╝       ╚═════╝ ╚══════╝╚═════╝ \n", invoice);
	fputs("\n", invoice);
	fputs("\n", invoice);

	fputs("MEGRENDELŐ ADATAI:\n", invoice);

	fputs("  Neve:   ", invoice);
	fputs(worksheet.order.name, invoice);
	fputs("\n", invoice);

	fputs("  Címe:   ", invoice);
	fputs(worksheet.order.address, invoice);
	fputs("\n", invoice);

	fputs("  Dátum:  ", invoice);
	fputs(date_to_str(time(NULL)), invoice);
	fputs("\n", invoice);

	char temp_str[5];

	fputs("\n", invoice);
	fputs("ÁR: 1000 CR * ", invoice);
	sprintf(temp_str, "%i", worksheet.time);
	fputs(temp_str, invoice);
	fputs(" = ", invoice);
	sprintf(temp_str, "%i", worksheet.time * 1000);
	fputs(temp_str, invoice);
	fputs(" CR\n", invoice);

	if (worksheet.order.priority)
	{
		fputs(" + felár:         ", invoice);
		sprintf(temp_str, "%i", worksheet.time * 500);
		fputs(temp_str, invoice);
		fputs(" CR\n", invoice);
		fputs("=========================\n", invoice);
		fputs("                  ", invoice);
		sprintf(temp_str, "%i", worksheet.time * 1500);
		fputs(temp_str, invoice);
		fputs(" CR\n", invoice);
	}

	fputs("\n", invoice);

	fclose(invoice);
}



// ========================================
//  TERMINAL HACKS
// ========================================

void tuneTerminal()
{
	tcgetattr(STDIN_FILENO, &oldt);
	newt = oldt;
	newt.c_lflag &= ~(ICANON | ECHO);
	tcsetattr(STDIN_FILENO, TCSANOW, &newt);
	oldf = fcntl(STDIN_FILENO, F_GETFL, 0);
	fcntl(STDIN_FILENO, F_SETFL, oldf | O_NONBLOCK);
}
void restoreTerminal()
{
	tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
	fcntl(STDIN_FILENO, F_SETFL, oldf);
}



// ========================================
//  VISUAL COMPONENTS
// ========================================

void print_box_top(int width)
{
	int i = 0;
	printf("╔");
	while (i < width - 2) {
		printf("═");
		i += 1;
	}
	printf("╗\n");
}
void print_box_bottom(int width)
{
	int i = 0;
	printf("╚");
	while (i < width - 2) {
		printf("═");
		i += 1;
	}
	printf("╝\n");
}
void print_box(int width, int height)
{
	print_box_top(width);

	int x, y = 0;
	while (y < height - 2) {
		x = 0;
		printf("║");
		while (x < width - 2) {
			printf(" ");
			x += 1;
		}
		printf("║\n");
		y += 1;
	}

	print_box_bottom(width);
}

void print_list_footer()
{
	char filter_prior_sign = ' ';

	if (filter_prior)
		filter_prior_sign = 'X';

	printf("╟──────────────────────────────────────────────────────────────────────────────╢\n");
	printf("║  [F3] Szűrés névre: %-30s         [F4] Sürgős? [%c]  ║\n", filter_str, filter_prior_sign);
}

void print_message_box()
{
	gotoxy(0, 0);
	setcolor(94, 40);
	print_box(80, 8);
	setcolor(37, 40);

	int i = 0;
	while (i < MSG_STACK_SIZE)
	{
		gotoxy(i + 2, 3);
		printf("%s\n", msg_stack_shared + (i * 65));
		i += 1;
	}
}
void print_list_box(int selected)
{
	gotoxy(9, 0);

	static int window_top = 0;
	static int window_bottom = 3;

	if (selected > window_bottom)
	{
		window_bottom = selected;
		window_top = window_bottom - 3;
	}
	else if (selected < window_top)
	{
		window_top = selected;
		window_bottom = window_top + 3;
	}

	print_box_top(80);
	printf("║   Név                             Email                            Sürgős?   ║\n");
	printf("║     Cím                                            Telefonszám       Dátum   ║\n");
	printf("╟──────────────────────────────────────────────────────────────────────────────╢\n");

	if (list_count == 0)
	{
		printf("║                                                                              ║\n");
		printf("║                                                                              ║\n");
		printf("║                                                                              ║\n");
		printf("║                             AZ ADATBÁZIS ÜRES                                ║\n");
		printf("║                                                                              ║\n");
		printf("║                                                                              ║\n");
		printf("║                                                                              ║\n");
		printf("║                                                                              ║\n");
	}

	int i = 0;
	int j = 0;
	while (i < visible_list_count)
	{
		if (i >= window_top && i <= window_bottom)
		{
			char *prior = "[ ]";
			if (visible_items[i].priority)
				prior = "[X]";

			printf("║ ");
			if (i == selected) {
				setcolor(37, 44);
			}
			printf("  %-30s  %-35s  %s  ", visible_items[i].name, visible_items[i].email, prior);
			setcolor(37, 40);
			printf(" ║\n");

			printf("║ ");
			if (i == selected) {
				setcolor(37, 44);
			}
			printf("    %-42s  %14s  %s  ", visible_items[i].address, visible_items[i].phone, date_to_str(visible_items[i].date));
			setcolor(37, 40);
			printf(" ║\n");

			j = j + 1;
		}

		i = i + 1;
	}

	while (list_count && j < 4)
	{
		printf("║                                                                              ║\n");
		printf("║                                                                              ║\n");
		j = j + 1;
	}
	print_list_footer();
	print_box_bottom(80);
}
void print_help()
{
	gotoxy(25, 0);

	setcolor(30, 47);
	printf("    INS: Beszúrás      DEL: Törlés      ENTER: Szerkesztés      ESC: Kilépés    ");
	setcolor(37, 40);
}

void print_home_screen(int selected)
{
	print_message_box();
	print_list_box(selected);
	print_help();
}

void print_screen_edit(int selected)
{
	int i = 0;
	int c = -1;

	while (i < list_count)
	{
		if (list_items[i].visible)
			c = c + 1;

		if (c == selected)
			break;

		i = i + 1;
	}

	flush_stdin();
	restoreTerminal();

	clrscr();
	printf("ASDFASDFASDF");
	print_message_box();

	gotoxy(9, 0);
	print_box(80, 14);

	setcolor(90, 40);
	gotoxy(11, 5);
	printf("Adja meg az új nevet: ");
	gotoxy(13, 5);
	printf("Adja meg az új címet: ");
	gotoxy(15, 5);
	printf("Adja meg az új email címet: ");
	gotoxy(17, 5);
	printf("Adja meg az új telefonszámot: ");
	gotoxy(19, 5);
	printf("Sürgős? [i/n] ");

	char ch;
	if (list_items[i].data.priority)
		ch = 'i';
	else
		ch = 'n';

	setcolor(37, 40);
	gotoxy(12, 7);
	printf("(%s)", list_items[i].data.name);
	gotoxy(14, 7);
	printf("(%s)", list_items[i].data.address);
	gotoxy(16, 7);
	printf("(%s)", list_items[i].data.email);
	gotoxy(18, 7);
	printf("(%s)", list_items[i].data.phone);
	gotoxy(20, 7);
	printf("(%c)", ch);

	char str[100];

	setcolor(33, 40);
	gotoxy(11, 27);
	fgets(str, 31, stdin);
	str[strlen(str) - 1] = '\0';
	if (str[0] != '\0')
		strcpy(list_items[i].data.name, str);

	gotoxy(13, 27);
	fgets(str, 51, stdin);
	str[strlen(str) - 1] = '\0';
	if (str[0] != '\0')
		strcpy(list_items[i].data.address, str);

	gotoxy(15, 33);
	fgets(str, 36, stdin);
	str[strlen(str) - 1] = '\0';
	if (str[0] != '\0')
		strcpy(list_items[i].data.email, str);

	gotoxy(17, 35);
	fgets(str, 36, stdin);
	str[strlen(str) - 1] = '\0';
	if (str[0] != '\0')
		strcpy(list_items[i].data.phone, str);

	gotoxy(19, 19);
	fgets(str, 2, stdin);
	if (str[0] != '\n')
	{
		if (toupper(str[0]) == 'I')
			list_items[i].data.priority = 1;
		else if (toupper(str[0]) == 'N')
			list_items[i].data.priority = 0;
		scanf("%*c");
	}

	tuneTerminal();
}
void print_screen_insert(t_item * new_item)
{
	char ch;

	gotoxy(9, 0);

	flush_stdin();
	restoreTerminal();

	print_box_top(80);
	printf("║                                                                              ║\n");
	printf("║   ");
	setcolor(90, 40);
	printf("Adja meg a megrendelő nevét: ");
	setcolor(37, 40);
	printf("                                              ║\n");
	printf("║                                                                              ║\n");
	printf("║   ");
	setcolor(90, 40);
	printf("Adja meg a megrendelő címét: ");
	setcolor(37, 40);
	printf("                                              ║\n");
	printf("║                                                                              ║\n");
	printf("║   ");
	setcolor(90, 40);
	printf("Adja meg a megrendelő email címét: ");
	setcolor(37, 40);
	printf("                                        ║\n");
	printf("║                                                                              ║\n");
	printf("║   ");
	setcolor(90, 40);
	printf("Adja meg a megrendelő telefonszámát: ");
	setcolor(37, 40);
	printf("                                      ║\n");
	printf("║                                                                              ║\n");
	printf("║   ");
	setcolor(90, 40);
	printf("Sürgős? [i/N] ");
	setcolor(37, 40);
	printf("                                                             ║\n");
	printf("║                                                                              ║\n");
	print_box_bottom(80);

	char str[100];

	setcolor(33, 40);
	new_item->data.date = time(NULL);

	gotoxy(11, 34);
	//	scanf("%[^\n]%*c", new_item->data.name);
	fgets(str, 31, stdin);
	str[strlen(str) - 1] = '\0';
	strcpy(new_item->data.name, str);

	gotoxy(13, 34);
	//	scanf("%[^\n]%*c", new_item->data.address);
	fgets(str, 46, stdin);
	str[strlen(str) - 1] = '\0';
	strcpy(new_item->data.address, str);

	gotoxy(15, 40);
	//	scanf("%[^\n]%*c", new_item->data.email);
	fgets(str, 36, stdin);
	str[strlen(str) - 1] = '\0';
	strcpy(new_item->data.email, str);

	gotoxy(17, 42);
	//	scanf("%[^\n]%*c", new_item->data.phone);
	fgets(str, 16, stdin);
	str[strlen(str) - 1] = '\0';
	strcpy(new_item->data.phone, str);

	gotoxy(19, 19);
	fgets(str, 2, stdin);
	if (str[0] != '\n' && toupper(str[0]) == 'I')
	{
		new_item->data.priority = 1;
		scanf("%*c");
	}
	else if (str[0] != '\n')
	{
		new_item->data.priority = 0;
		scanf("%*c");
	}
	else
	{
		new_item->data.priority = 0;
	}
	//while ((ch = getchar()) && !(ch == 'i' || ch == 'I' || ch == 'n' || ch == 'N'));
	//ch = getchar();
	//if (toupper(ch) == 'I')
	//	new_item->data.priority = 1;
	//else
	//	new_item->data.priority = 0;
	//scanf("%*c");

	tuneTerminal();
}
void print_filter_dialog()
{
	setcolor(37, 41);

	gotoxy(12, 20);
	print_box_top(40);

	gotoxy(13, 20);
	printf("║                                      ║");

	gotoxy(14, 20);
	printf("║  Szűrés névre: ");
	setcolor(37, 40);
	printf("                    ");
	setcolor(37, 41);
	printf("  ║");

	gotoxy(15, 20);
	printf("║                                      ║");

	gotoxy(16, 20);
	print_box_bottom(40);

	flush_stdin();
	restoreTerminal();
	gotoxy(14, 37);
	setcolor(37, 40);
	//scanf("%[^\n]", filter_str);
	fgets(filter_str, 20, stdin);
	filter_str[strlen(filter_str) - 1] = '\0';

	tuneTerminal();
	flush_stdin();
}



// ========================================
//  MESSAGE STACK FUNCTIONS
// ========================================

void send_message(const char * message)
{
	time_t rawtime;
	time(&rawtime);
	char * time_str = time_to_str(rawtime);

	t_msg msg;
	msg.mtype = 1;
	strcpy(msg.mtext, "[");
	strcat(msg.mtext, time_str);
	strcat(msg.mtext, "] ");
	strcat(msg.mtext, message);
	if (msgsnd(msg_queue, &msg, sizeof(msg.mtext), 0) < 0)
		perror("Üzenetküldési hiba");
}
void post_message(const char * message)
{
	if (msg_stack_size < MSG_STACK_SIZE) {
		//strcpy(msg_stack[msg_stack_size], message);

		strcpy(msg_stack_shared + (msg_stack_size * 65), message);

		msg_stack_size += 1;
		print_message_box();
		return;
	}

	int i = 0;
	while (i < MSG_STACK_SIZE - 1)
	{
		strcpy(msg_stack_shared + (i * 65), msg_stack_shared + ((i + 1) * 65));
		i += 1;
	}
	strcpy(msg_stack_shared + (i * 65), message);

	print_message_box();
}



// ========================================
//  LIST FUNCTIONS
// ========================================

void filter_list()
{
	int i = 0;
	int c = 0;

	visible_list_count = 0;

	while (i < list_count)
	{
		if (stristr3(list_items[i].data.name, filter_str) != NULL && (list_items[i].data.priority || filter_prior == 0)) {
			list_items[i].visible = 1;
			visible_list_count += 1;
		} else {
			list_items[i].visible = 0;
		}

		i += 1;
	}

	visible_items = (t_order *) malloc( visible_list_count * sizeof(t_order) );

	i = 0;
	while (i < list_count)
	{
		if (list_items[i].visible) {
			visible_items[c] = list_items[i].data;
			c += 1;
		}

		i += 1;
	}
}
void insert_new_item(t_item * new_item)
{
	list_count += 1;
	list_items = (t_item *)realloc(list_items, list_count * sizeof(t_item));
	list_items[list_count - 1] = *new_item;
}
void del_selected_item(int item_number)
{
	int i = 0;
	int c = 0;

	while (i < list_count - 1)
	{
		if (list_items[i].visible)
			c += 1;

		if (c >= item_number)
			list_items[i] = list_items[i + 1];

		i += 1;
	}

	list_count -= 1;
	list_items = (t_item *) realloc(list_items, list_count * sizeof(t_item));
}
void del_item(int item_number)
{
	int i = item_number;

	while (i < list_count - 1)
	{
		list_items[i] = list_items[i + 1];
		i += 1;
	}

	list_count -= 1;
	list_items = (t_item *) realloc(list_items, list_count * sizeof(t_item));
}



// ========================================
//  DATABASE FUNCTIONS
// ========================================

void read_database()
{
	FILE *db_file = fopen("database.db", "rb");

	if (db_file != NULL)
	{
		fseek(db_file, 0L, SEEK_END);
		list_count = ftell(db_file) / sizeof(t_item);
		list_items = (t_item *) realloc(list_items, list_count * sizeof(t_item));
		rewind(db_file);
		fread(list_items, sizeof(t_item), list_count, db_file);
		fclose(db_file);

		int i = 0;
		while (i < list_count)
		{
			list_items[i].visible = 1;
			i = i + 1;
		}
	}
	else
	{
		perror("Adatbázis megnyitási hiba");
	}
}
void write_database()
{
	FILE *db_file = fopen("database.db", "wb");

	if (db_file != NULL)
	{
		fwrite(list_items, sizeof(t_item), list_count, db_file);
		fclose(db_file);
	}
}



// ========================================
//  PROCESSES HANDLERS
// ========================================

void handler_main(int signum)
{
	if (signum == SIGUSR1)
	{
		send_message("Madár megetetve.");
	}
	else if (signum == SIGUSR2)
	{
		read(pipe_idomar[0], &worksheet, sizeof(t_ws));

		make_invoice(worksheet.order);

		char msg[] = "Számla elkészült: ";
		strcat(msg, worksheet.order.name);
		//strcat(msg, "  ");
		//char a[2];
		//sprintf(a, "%i", worksheet.idx);
		//strcat(msg, a);
		send_message(msg);

		del_item(worksheet.idx);
		write_database();
		filter_list();
		print_home_screen(0);
	}
}
void handler_madar(int signum, siginfo_t *info, void *context)
{
	int catch_time = rand() % 5 + 1;
	
	sleep(catch_time);

	union sigval value;
	value.sival_int = catch_time;
	sigqueue(info->si_pid, SIGUSR1, value);
}
void handler_idomar(int signum, siginfo_t *info, void *context)
{
	char time[1];
	sprintf(time, "%i", info->si_value.sival_int);
	char msg[] = "[IDOMÁR] A befogás sikerült ";
	strcat(msg, time);
	strcat(msg, " másodperc alatt.");
	send_message(msg);
	worksheet.time = info->si_value.sival_int;
	write(pipe_idomar[1], &worksheet, sizeof(t_ws));
	kill(getppid(), SIGUSR2);
}



// ========================================
//  PROCESSES
// ========================================

// Process: MESSAGES
void ProcessMessages()
{
	while (1)
	{
		t_msg message;
		msgrcv(msg_queue, &message, sizeof(message.mtext), 1, 0);
		post_message(message.mtext);
	}
}

// Process: IDOMAR
void ProcessIdomar()
{
	struct sigaction sa;

	sa.sa_flags = SA_SIGINFO;
	sa.sa_sigaction = handler_idomar;
	sigemptyset(&sa.sa_mask);
	sigaction(SIGUSR1, &sa, NULL);

	close(pipe_main[1]);
	close(pipe_idomar[0]);

	// "Feeding bird"
	sleep(1);
	kill(getppid(), SIGUSR1);

	while (1)
	{
		read(pipe_main[0], &worksheet, sizeof(t_ws));

		char msg[] = "[IDOMAR] A címet megkaptam: ";
		strcat(msg, worksheet.order.name);
		send_message(msg);

		kill(p_madar, SIGUSR1);

		pause();
	}
}

// Process: MADAR
void ProcessMadar()
{
	struct sigaction sa;

	sa.sa_flags = SA_SIGINFO;
	sa.sa_sigaction = handler_madar;
	sigemptyset(&sa.sa_mask);
	sigaction(SIGUSR1, &sa, NULL);
	
	while (1) { pause(); }
}

// Process: MAIN
void ProcessMain()
{
	signal(SIGUSR1, handler_main);
	signal(SIGUSR2, handler_main);

	close(pipe_main[0]);
	close(pipe_idomar[1]);

	clrscr();

	read_database();
	filter_list();

	print_home_screen(0);

	tuneTerminal();

	int ch, i = 0;
	while (1)
	{
		ch = getchar();
		if (ch == 27)
		{
			ch = getchar();
			if (ch == 91) {
				ch = getchar();
				if (ch == 65 && i > 0) // UP
				{
					i -= 1;
					print_home_screen(i);
				}
				else if (ch == 66 && i < visible_list_count - 1) // DOWN
				{
					i += 1;
					print_home_screen(i);
				}
				else if (ch == 50) // INS
				{
					t_item *new_item = (t_item*) malloc(sizeof(t_item));

					clrscr();
					print_message_box();
					print_screen_insert(new_item);
					insert_new_item(new_item);
					free(new_item);
					write_database();
					filter_list();
					print_home_screen(i);
				}
				else if (ch == 51) // DEL
				{
					del_selected_item(i);
					write_database();
					filter_list();
					if (i >= visible_list_count)
						i = visible_list_count - 1;
					print_home_screen(i);
				}
				else if (ch == 49)
				{
					ch = getchar();
					if (ch == 51) // F3
					{
						print_filter_dialog();
						filter_list();
						i = 0;
						print_home_screen(i);
					}
					else if (ch == 52) // F4
					{
						filter_prior = (filter_prior + 1) % 2;
						filter_list();
						if (i >= visible_list_count)
							i = visible_list_count - 1;
						print_home_screen(i);
					}
					else if (ch == 53) // F5
					{
						if (list_count > 0)
						{
							int idx = 0, j = 0;

							while (j < list_count)
							{
								if (list_items[j].data.priority)
								{
									idx = j;
									break;
								}
								j += 1;
							}

							worksheet.order = list_items[idx].data;
							worksheet.idx = idx;
							worksheet.time = 0;
							write(pipe_main[1], &worksheet, sizeof(t_ws));
						}
						else
						{
							send_message("Minden rendelés teljesítve!");
						}

					}
				}
			}
			else // ESC
			{
				break;
			}
		}
		else if (ch == 10) // ENTER
		{
			print_screen_edit(i);
			write_database();
			filter_list(i);
			print_home_screen(i);
		}
	}

	restoreTerminal();
}



int main(int argc, char *argv[])
{
	// Init random number generator
	srand(time(NULL));


	// Creating message queue
	//msg_queue = msgget( ftok("progfile", 65), 0666 | IPC_CREAT );
	msg_queue = msgget(IPC_PRIVATE, 0666 | IPC_CREAT);
	if (msg_queue < 0) {
		perror("msgget");
		return 1;
	}


	// Creating shared memory for message queue
	msg_stack_shared_id = shmget(ftok(argv[0], 1), sizeof(msg_stack), IPC_CREAT | S_IRUSR | S_IWUSR);
	msg_stack_shared = shmat(msg_stack_shared_id, NULL, 0);


	// Creating pipe
	pipe(pipe_main);
	pipe(pipe_idomar);


	// Creating process: MADAR
	p_madar = fork();
	if (p_madar < 0) {
		perror("MADAR process létrehozási hiba");
		exit(1);
	} else if (p_madar == 0) {
		ProcessMadar();
		exit(0);
	}

	// Creating process: IDOMAR
	p_idomar = fork();
	if (p_idomar < 0) {
		perror("IDOMAR process létrehozási hiba");
		exit(1);
	} else if (p_idomar == 0) {
		ProcessIdomar();
		exit(0);
	}

	// Creating process: MESSAGES
	p_msg = fork();
	if (p_msg < 0) {
		perror("MESSAGES process létrehozási hiba");
		exit(1);
	} else if (p_msg == 0) {
		ProcessMessages();
		exit(0);
	}

	ProcessMain();


	clrscr();


	// Terminating processes
	kill(p_msg,		SIGKILL);
	kill(p_madar,	SIGKILL);
	kill(p_idomar,	SIGKILL);


	// Deleting message queue
	msgctl(msg_queue, IPC_RMID, NULL);


	// Deleting shared memory
	shmctl(msg_stack_shared_id, IPC_RMID, NULL);


	return 0;
}
