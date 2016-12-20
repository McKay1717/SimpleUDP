#include<stdio.h>
#include<string.h>
#include<stdlib.h>
#include<arpa/inet.h>
#include<sys/socket.h>
#include <unistd.h>
#include <netdb.h>
#define BUFLEN 512

void die(char *s) {
	perror(s);
	exit(1);
}

int main(int argc, char *argv[]) {

	struct sockaddr_in si_other;
	struct hostent *hostentry;
	int s, i = sizeof(si_other);
	socklen_t slen;
	char buf[BUFLEN] ="";
	char *message;
	int port = 8888;
	char server[255];

	//Verification des args
	if (argc < 4) {
		printf("Usage: %s adresse_serveur port_serveur mots... \n", argv[0]);
		exit(1);
	} else {
		//Attribution su parametre serveur
		strcpy(server, argv[1]);
		//Resolution de l'ip
		int ret = gethostname(server, sizeof(server));
		//Si le nom n'est pas resolvable on sort
		if (-1 == ret) {
			perror("Nom non resolvable");
			exit(1);
		}
		hostentry = gethostbyname(server);
		//Si le nom n'est pas resolvable on sort
		if (NULL == hostentry) {
			perror("Nom non resolvable");
			exit(1);
		}
		char * ipbuf = inet_ntoa(
				*((struct in_addr *) hostentry->h_addr_list[0]));

		if (NULL == ipbuf) {
			perror("inet_ntoa");
			exit(1);
		}
		strcpy(server, ipbuf);

		port = atoi(argv[2]);
		//Calcul de la taille de "mots..."
		int size = argc - 3;
		for (i = 3; i < argc; i++) {
			size += sizeof(argv[i]);
		}
		//Concatenation du reste des arguments
		message = (char *) malloc(size);
		//Initilisation de l'espace memoire
		memset(message, 0,size);
		for (i = 3; i < argc; i++) {
			strcat(message, argv[i]);
			strcat(message, " ");
		}
	}
	//Ouverture de la socket
	if ((s = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) == -1) {
		die("socket");
	}

	//Initialisation des structures
	memset((char *) &si_other, 0, sizeof(si_other));
	si_other.sin_family = AF_INET;
	si_other.sin_port = htons(port);

	//Definition de la destination
	if (inet_aton(server, &si_other.sin_addr) == 0) {
		fprintf(stderr, "Impossible de convertir l'ip");
		return -1;
	}

	//Envoie du message
	slen = sizeof(si_other);
	if (sendto(s, message, strlen(message), 0, (struct sockaddr *) &si_other,
			slen) == -1) {
		die("Impossible d'envoyer le message");
	}

	//Preparation du buffer
	memset(buf, '\0', BUFLEN);
	//reception de la reponse (methode bloquante)
	if (recvfrom(s, buf, BUFLEN, 0, (struct sockaddr *) &si_other, &slen)
			== -1) {
		die("Erreur de la reception de la reponse");
	}

	//Affichage du retour
	puts(buf);
	//Fermeture de la socket et liberation de la memoire

	close(s);
	free(message);
	return 0;
}

