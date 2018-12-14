#include "20161620.h"//라이브러리,전역변수,매크로
///////////////////함수 주석은 헤더파일에////////////////////////
///////////////////반복해서 쓰이는 함수는 헤더파일에 작성함///////////////////
int main (void)
{
    DIR* dir;//디렉토리
    struct dirent* ent;
    struct stat buf;
    char home[1024];
    int i,j;
    int isRightCommand=1;
    int fileFlag = 0;
    signed long temp;
    FILE *fp;
    unsigned int id;
    char file_Name_temp[MAX];
    Node* pnode = NULL;//출력때 이용할 포인터
    Table* new = NULL;
    symbolTab tempTab;
    int is_load;//로드가 정상적으로 수행되었는지 반환값을 받음
    int obj1_fileFlag = 1;
    int obj2_fileFlag = 1;
    int obj3_fileFlag = 1;//해당obj파일이 디렉토리에 있는지 검사
    unsigned int total_len;

    while(1)
    {
	for(i=0; i<MAX; i++)
	{
	    command[i] = '\0';
	    mnemonic[i] = '\0';
	    fileName[i] = '\0';
	    fileName_assem[i] = '\0';
	    fileStr[i] = '\0';
	    file_Name_temp[i] = '\0';
	    obf1[i] = '\0';
	    obf2[i] = '\0';
	    obf3[i] = '\0';
	}

	printf("\nsicsim> ");
	fgets(command,MAX,stdin);//공백까지 읽으려고 fgets 사용
	strcpy(store_command,command);
	store_command[strlen(store_command)-1]='\0';
	start = end = value = address = -1;
	fileFlag = 0;
	is_load = 1;
	///opcode 파일처리///
	fp = fopen("opcode.txt","r");
	while(1)
	{
	    if(fscanf(fp,"%X %s %s\n",&key,name,format)==EOF)
		break;
	    else
	    {
		new = Create_Table(key,name,format);
		if(hashTable[HASH_KEY(key)] == NULL)
		    hashTable[HASH_KEY(key)] = new;
		else
		{
		    new->hashNext = hashTable[HASH_KEY(key)];
		    hashTable[HASH_KEY(key)] = new;
		}
	    }
	}
	fclose(fp);//파일 읽어서 테이블 만들기
	
	isRightCommand = arrangeCommand(command);

	if(isRightCommand == -1)
	{
	    printf("error : 알맞은 형식의 입력(명령어)이 아닙니다. 다시 입력받으세요.\n");
	    for(i=0; i<MAX_HASH; i++)
		hashTable[i] = NULL;//해쉬테이블은 프리하고 돌아가기
	    return_flag2 = 1;
	    continue;
	}//이상한 문자 들어오면 루프위로가서 다시 시작;;history에도 안넣어주는게 처리된다!!

	if((!strcmp(command,"help")) || (!strcmp(command,"h")))
	{
	    insertBack(&head,&tail,Create_Node(store_command));
	    printf("\nh[elp]\nd[ir]\nq[uit]\nhi[story]\n");
	    printf("du[mp] [start, end]\n");
	    printf("e[dit] address, value\n");
	    printf("f[ill] start, end, value\n");
	    printf("reset\nopcode mnemonic\nopcodelist\n");
	    printf("assemble filename\ntype filename\nsymbol\n\n");
	}

	if((!strcmp(command,"dir")) || (!strcmp(command,"d")))
	{
	    insertBack(&head,&tail,Create_Node(store_command));
	    strncpy(home,getenv("HOME"),sizeof(home));//로그인한 랜덤한 사용자의 홈을 받기 위함
	    dir = opendir(home);
	    if(dir != NULL)
	    {
		printf("\n");
		while((ent = readdir(dir)) != NULL)
		{
		    stat(ent->d_name,&buf);
		    if(S_ISDIR(buf.st_mode))
			printf("%s/\n",ent->d_name);
		    else if((buf.st_mode & S_IXUSR) != 0)
			printf("%s*\n",ent->d_name);
		    else
			printf("%s\n",ent->d_name);
		}
		printf("\n");
		closedir(dir);
	    }
	    else
	    {
		perror("");
		return EXIT_FAILURE;
	    }
	}

	if((!strcmp(command,"history")) || (!strcmp(command,"hi")))
	{
	    insertBack(&head,&tail,Create_Node(store_command));
	    i=1;
	    pnode=head;
	    while(pnode != NULL)
	    {
		printf("%5d :%s\n",i,pnode->data);
		pnode = pnode->link;
		i++;
	    }
	}

	if(((!strcmp(command,"dump")) || (!strcmp(command,"du"))))
	{
	    insertBack(&head,&tail,Create_Node(store_command));
	    if(line_num>=1048560)
	    {
		line_num = 0;
		dump_p = 0;
	    }//이미 0xFFFFF값 넘었을때 다시 0부터 출력 처리

	    if((dump_p % 16) == 0)
	    {
		temp = 0;
		for(i=0; i<10; i++)
		{
		    if(line_num>1048560)
			break;
		    printf("%05X ",line_num);
		    for(j=0; j<16; j++)
		    {
			printf("%02X ",virtual[dump_p]);
			dump_p++;
		    }
		    printf("; ");
		    dump_p = dump_p - 16;
		    for(j=0; j<16; j++)
		    {
			if((virtual[dump_p]>=0x20) && (virtual[dump_p]<=0x7E))
			    printf("%c",virtual[dump_p]);
			else
			    printf(".");
			dump_p++;
		    }
		    printf("\n");
		    line_num+=16;
		}
	    }
	    else
	    {////dump만 쳤을때 첫번째 번지부터 출력하지 않는 경우도 처리
		temp = dump_p%16;
		line_num = (unsigned int)(dump_p-temp);
		for(i=0; i<10; i++)
		{
		    if(line_num > 1048560)
			break;
		    printf("%05X ",line_num);
		    if(i==0)
		    {
			for(j=0; j<temp; j++)
			    printf("   ");
			for(j=temp; j<16; j++)
			{
			    printf("%02X ",virtual[dump_p]);
			    dump_p++;
			}
			printf("; ");
			dump_p = dump_p-(unsigned int)(16-temp);
			for(j=0; j<temp; j++)
			printf(".");
			for(j=temp; j<16; j++)
			{
			    if((virtual[dump_p]>=0x20) && (virtual[dump_p]<=0x7E))
				printf("%c",virtual[dump_p]);
			    else
				printf(".");
			    dump_p++;
			}	
			printf("\n");
		    }
		    else
		    {
			for(j=0; j<16; j++)
			{
			    printf("%02X ",virtual[dump_p]);
			    dump_p++;
			}
			printf("; ");
			dump_p = dump_p - 16;
			for(j=0; j<16; j++)
			{
			    if((virtual[dump_p]>=0x20) && (virtual[dump_p]<=0x7E))
				printf("%c",virtual[dump_p]);
			    else
				printf(".");
			    dump_p++;
			}
			printf("\n");
		    }
		    line_num+=16;
		}
		if(line_num <= 1048560 && temp!=0)
		{
		    printf("%05X ",line_num);
		    for(i=0; i<temp; i++)
		    {
			printf("%02X ",virtual[dump_p]);
			    dump_p++;
		    }
		    for(i=temp; i<16; i++)
			printf("   ");
		    printf("; ");
		    dump_p = dump_p - temp;
		    for(i=0; i<temp; i++)
		    {
			if((virtual[dump_p]>=0x20) && (virtual[dump_p]<=0x7E))
			    printf("%c",virtual[dump_p]);
			else
			    printf(".");
		    dump_p++;
		    }
		    for(i=temp; i<16; i++)
			printf(".");
		    printf("\n");//160개 출력을 위한 처리
		}
	    }
	}
	
	if(start != -1 || end != -1 || value != -1 || address != -1)
	{   
	    DumpEditFill(start,end,address,value);//dump , fill, edit
	}

	if((!strcmp(command,"reset")))
	{      
	    insertBack(&head,&tail,Create_Node(store_command));
	    temp_p = dump_p;//임시 저장
	    dump_p=0;
	    for(i=0; i<MAX_memory; i++)
	    {
		virtual[dump_p] = 0;
		dump_p++;
	    }
	    dump_p = temp_p;//원상복구
	}
	
	if((!strcmp(command,"opcodelist")))
	{
	    insertBack(&head,&tail,Create_Node(store_command));
	    for(i=0; i<MAX_HASH; i++)
	    {
		printf("%d : ",i);
		if(hashTable[i] != NULL)
		{
		    new = hashTable[i];
		    while(new->hashNext)
		    {
			printf("[%s,%X]",new->instruct,new->id);
			new = new->hashNext;
			printf(" -> ");
		    }
		    printf("[%s,%X]",new->instruct,new->id);
		}
		printf("\n");
	    }
	}

	if(strlen(mnemonic) != 0)
	{
	    id = hashFunc(mnemonic);
	    if(id<MAX_HASH)
	    {
		insertBack(&head,&tail,Create_Node(store_command));
		if(!strcmp(hashTable[id]->instruct,mnemonic))
		    printf("opcode is %X\n",hashTable[id]->id);
		else
		{
		    new = hashTable[id];
		    while(new->hashNext)
		    {
			if(!strcmp(new->hashNext->instruct,mnemonic))
			    printf("opcode is %X\n",new->hashNext->id);
			new = new->hashNext;
		    }
		}
	    }
	    else
		printf("error! : 입력한 Mnemonic은 테이블에 없습니다.\n");
	}

	if(strlen(fileName) != 0)
	{
	    insertBack(&head,&tail,Create_Node(store_command));

	    dir = opendir("./");
	    if(dir != NULL)
	    {
		while((ent = readdir(dir)) != NULL)
		{
		    if(!strcmp(ent->d_name,fileName))//디렉토리에 파일 있으면
		    {
			printf("\n");
			///파일 읽어서 출력///
			fp = fopen(fileName,"r");
			while(fgets(fileStr,MAX-1,fp) != NULL)
			{
			    fileStr[strlen(fileStr)-1] = '\0';//끝에 개행문자 붙는거 지움
			    puts(fileStr);
			}
			fileFlag = 1;
			fclose(fp);
			break;
		    }
		    else
			continue;
		}
		if(fileFlag == 0)
		    printf("error : 현재 디렉토리에 파일이 없습니다.\n");
		closedir(dir);
	    }
	    else
	    {
		perror("");
		return EXIT_FAILURE;
	    }
	}

	if(strlen(fileName_assem) != 0)
	{
	    for(i=0; i<MAX; i++)
		memset(&symTab, i, sizeof(struct symbolTab));//assemble 명령어 실행시킬때 symtab 초기화한다
	    insertBack(&head,&tail,Create_Node(store_command));
	    if(strstr(fileName_assem,".asm"))
	    {
		strcpy(fileName, fileName_assem);
		for(i=strlen(fileName_assem)-1; ; i--)
		{
		    if(fileName_assem[i] == '.')
		    {
			fileName_assem[i] = '\0';
			break;
		    }
		    else
			fileName_assem[i] = '\0';
		}//원래 파일 이름은 fileName에 저장하고 확장자 뗀 파일 이름만 fileName_assem에 저장

		strcpy(file_Name_temp,fileName_assem);
		//리스트파일과 오브젝파일 생성, 출력
		
		if(makeLST(fileName_assem) != -1)
		    printf("\noutput file : [%s.lst], [%s.obj]\n",file_Name_temp,file_Name_temp);
		//파일 닫기 처리하기
	    }
	    else
		printf("error : .asm 확장자를 가진 파일만 assemble 할 수 있습니다.\n");
	}

	if(!strcmp(command,"symbol"))
	{
	    insertBack(&head,&tail,Create_Node(store_command));
	    for(i=0; i < symTab_i; i++)
	    {
		for(j = 0; j < symTab_i - 1; j++)
		{
		    if(symTab[j].symbol[0] < symTab[j+1].symbol[0])
		    {
			tempTab = symTab[j];
			symTab[j] = symTab[j+1];
			symTab[j+1] = tempTab;
		    }
		}
	    }//내림차순 정렬

	    i=0;
	    while(1)
	    {
		printf("\t%s\t%04X",symTab[i].symbol,symTab[i].loc);
		i++;
		if(strlen(symTab[i].symbol) !=0)
		    printf("\n");
		else
		    break;//마지막 개행 처리를 위해 개행작업 따로`
	    }
	}
/*
	if(strstr(command,"progaddr"))
	{
	    printf("-  program address를 %#x 주소로 지정합니다.\n",progaddr);
	    printf("-  loader 또는 run 명령어의 수행이 %#x 부터 시작합니다.\n",progaddr);
	}
*/
	//progaddr의 명령어 수행 시 인자가 있으면 주소를 바꿔주기만 하면 되고 별도 처리 없다. progaddr은 전역변수
	if(strstr(command,"loader"))
	{
	    if((strlen(obf1) != 0 && !strstr(obf1,".obj")) || (strlen(obf2) != 0 && !strstr(obf2,".obj")) || (strlen(obf3) != 0 && !strstr(obf3,".obj")))
		printf("error! : 입력 파일이 object 프로그램이 아닙니다.\n");//입력한 파일중에 .obj확장자가 아닌 파일일 경우 로드 수행X
	    else
	    {
		estab_i = 0;
		total_len = 0;
		for(i=0; i<MAX; i++)
		{
		    strcpy(estab[i].csname,"\0");
		    strcpy(estab[i].symname,"\0");
		    estab[i].addr = 0;
		    estab[i].len = 0;
		}//load 할때마다 초기화

		if(strlen(obf1) != 0)
		{
		    obj1_fileFlag = 0;
		    dir = opendir("./");
		    if(dir != NULL)
		    {
			while((ent = readdir(dir)) != NULL)
			{
			    if(!strcmp(ent->d_name,obf1))//디렉토리에 파일 있으면
			    {
				obj1_fileFlag = 1;
				break;
			    }
			    else
				continue;
			}   
			if(obj1_fileFlag == 0)
			{
			    printf("error! : 현재 디렉토리에 해당 object파일이 없습니다.\n");
			  //  return EXIT_FAILURE;
			}
			else if(obj1_fileFlag == 1)
			    is_load = pass1_load(obf1);//정상 파일의 경우 pass1 수행
			closedir(dir);
		    }
		    else
		    {
			perror("");
			return EXIT_FAILURE;
		    }
		}

		if(obj1_fileFlag!=0 && strlen(obf2) != 0)
		{
		    obj2_fileFlag = 0;
		    dir = opendir("./");
		    if(dir != NULL && (strstr(obf2,".obj")))
		    {
			while((ent = readdir(dir)) != NULL)
			{
			    if(!strcmp(ent->d_name,obf2))//디렉토리에 파일 있으면
			    {
				obj2_fileFlag = 1;
				break;
			    }
			    else
				continue;
			}   
			if(obj2_fileFlag == 0)
			{
			    printf("error! : 현재 디렉토리에 해당 object파일이 없습니다.\n");
			   // return EXIT_FAILURE;
			}
			else if(obj2_fileFlag == 1)
			    is_load = pass1_load(obf2);
			closedir(dir);
		    }
		    else
		    {
			perror("");
			return EXIT_FAILURE;
		    }
		}

		if(obj1_fileFlag!=0 && obj2_fileFlag!=0 && strlen(obf3) != 0)
		{
		    obj3_fileFlag = 0;
		    dir = opendir("./");
		    if(dir != NULL && (strstr(obf1,".obj")))
		    {
			while((ent = readdir(dir)) != NULL)
			{
			    if(!strcmp(ent->d_name,obf3))//디렉토리에 파일 있으면
			    {
				obj3_fileFlag = 1;
				break;
			    }
			    else
				continue;
			}   
			if(obj3_fileFlag == 0)
			{
			    printf("error! : 현재 디렉토리에 해당 object파일이 없습니다.\n");
			  //  return EXIT_FAILURE;
			}
			else if(obj3_fileFlag == 1)
			    is_load = pass1_load(obf3);
			closedir(dir);
		    }
		    else
		    {
			perror("");
			return EXIT_FAILURE;
		    }
		}//해당 파일이 빈 파일이 아니며 디렉토리에 있으면 load 수행

		if(is_load != 1)
		    printf("error! : load가 정상적으로 수행되지 않았습니다.\n");//loader 수행 중 에러나면 예외처리
		else if(obj1_fileFlag != 0 && obj2_fileFlag != 0 && obj3_fileFlag != 0)
		{//로드가 정상적으로 수행되었고 파일이 디렉토리에 존재하며 정상 파일일 경우, 각 입력이 있는것에 대해 load pass2 수행
		    if(strlen(obf1) != 0)
			is_load = pass2_load(obf1);
		    if(strlen(obf2) != 0)
			is_load = pass2_load(obf2);
		    if(strlen(obf3) != 0)
			is_load = pass2_load(obf3);
		}

		if((is_load != -1) && (obj1_fileFlag != 0) && (obj2_fileFlag != 0) && (obj3_fileFlag !=0))
		{
		    printf("\n");
		    printf("control      symbol      address      length\n");
		    printf("section      name\n");
		    printf("-----------------------------------------------\n");
		    for(i=0; i<estab_i; i++)
		    {
			if(strlen(estab[i].csname) == 0)//symbol의 경우 len이 아예 출력되면 안되므로 경우 나누어줌
			    printf("%-13s%-10s  %04X\n",estab[i].csname,estab[i].symname,estab[i].addr);
			else
			    printf("%-13s%-10s	 %04X  	      %04X\n",estab[i].csname,estab[i].symname,estab[i].addr,estab[i].len);
			total_len += estab[i].len;
		    }
		    printf("-----------------------------------------------\n");
		    printf("                         total length	%04X\n",total_len);
		} //로드 끝난 후 load map 화면에 출력
	    }
	}
	    
	if((!strcmp(command,"quit")) || (!strcmp(command,"q")))
	{
	    insertBack(&head,&tail,Create_Node(store_command));
	    break;
	}

	for(i=0; i<MAX_HASH; i++)
	    hashTable[i] = NULL;//해시테이블 다시 채우기위해 초기화

	free(new);
	free(pnode);
    }
    printf("\n");
    return 0;
}
int arrangeCommand(char* command)
{
    int i,j = 0;
    int start_flag = 0;
    int return_flag1 = 1;
    int comma_index1=-1;
    int comma_index2=-1;
    int filenum = 0;
    int word_flag = 0;
    char store_obf[MAX] = {'\0',};
    char store[MAX] = {'\0',};
    char* ptr;
    
    command[(strlen(command)-1)] = '\0';//fgets로 받았으니까 우선 개행 문자 짜름

    cut_space(command);//앞뒤 공백 있는것 자름
    strcpy(store2,command);//앞뒤 없고 사이에는 공백 있는것

    for(i=0; (unsigned)i<strlen(command); i++)
    {
	if(command[i]==' ' || command[i]=='\t')
	    continue;
	if(command[i]>='A' && command[i]<='Z')
	    command[i] += 32;
	store[j] = command[i];
	j++;
    }
    strcpy(command,store);
    ////여기서부터는 dump에서 뒤에 숫자 명령어 나올 경우를 위한 별도 처리/////
    if(strstr(command,"dump") || strstr(command,"du"))
    {
	if(store2[0]=='d' && store2[1]=='u' && store2[2]=='m' && store2[3]=='p')
	{
	    i = check_store2("dump",store2);
	    if(return_flag2 == 2)//command=="dump"에서 끝날 때 바로 실행
		return 1;
	}
	else if(store2[0]=='d' && store2[1]=='u')
	{
	    i = check_store2("du",store2);
	    if(return_flag2 == 2)//command=="du"에서 끝날 때 바로 실행
		return 1;
	}
	else
	    return_flag2 = -1;/////////////이 과정에서 정상 명령어라고 가정했을 때 다음에 붙은 다른 문자가 있는지, 없고 공백만 있는지 확인한다.

    	strcpy(dumpRange,store2+i+1);//dump 명령어 이후의 인자만 dumpRange에 저장
	cut_space(dumpRange);//dumpRange 앞뒤 공백 있으면 자름
    
	for(i=0; (unsigned)i<strlen(dumpRange); i++)
	{
	    if(dumpRange[i] == ',')
		comma_index1=i;
	}
	if(comma_index1==-1) //콤마가 없으면;; start만 있으면
	{
	    for(i=0; (unsigned)i<strlen(dumpRange); i++)
	    {
		if((dumpRange[i]>='a' && dumpRange[i]<='f') || (dumpRange[i]>='A' && dumpRange[i]<='F') || (dumpRange[i]>='0' && dumpRange[i]<='9'))
		    continue;
		else
		{
		    return_flag2=-1;
		    break;
		}
	    }
	    
	    if(return_flag2 != -1)
	    {
		start = strtoul(dumpRange,NULL,16);
		return_flag2 = 1;
	    }
	}
	else
	{///콤마가 있을 때
	    start_flag=0;
	    for(i=comma_index1-1; i>=0; i--)
	    {
		if(start_flag==0 && ((dumpRange[i] != ' ') && (dumpRange[i] != '\t')))
		    start_flag=1;
		if(start_flag==1)
		{
		    if((dumpRange[i]>='a' && dumpRange[i]<='f') || (dumpRange[i]>='A' && dumpRange[i]<='F') || (dumpRange[i]>='0' && dumpRange[i]<='9'))
			continue;
		    else
		    {
			return_flag2=-1;
			break;
		    }
		}
	    }////콤마 앞에서 정상명령어 아닌것 처리

	    start_flag=0;
	    for(i=comma_index1+1; (unsigned)i<strlen(dumpRange); i++)
	    {      
		if(start_flag==0 && ((dumpRange[i] != ' ') && (dumpRange[i] != '\t')))
		    start_flag=1;
		if(start_flag==1)
		{
		    if((dumpRange[i]>='a' && dumpRange[i]<='f') || (dumpRange[i]>='A' && dumpRange[i]<='F') || (dumpRange[i]>='0' && dumpRange[i]<='9'))
			continue;
		    else
		    {
			return_flag2=-1;
			break;
		    }
		}
	    }////콤마 뒤에서 정상명령어 아닌것 처리
	}

	if(comma_index1 != -1 && return_flag2 != -1)
	{
	    ptr = strtok(dumpRange,",");//콤마를 기준으로 쪼갬
	    i=1;
	    while(ptr!=NULL)
	    {
		if(i==1)
		    start = strtoul(ptr,NULL,16);//문자열을 16진수로 바꾸어서 변수에 저장
		else if(i==2)
		    end = strtoul(ptr,NULL,16);
		ptr = strtok(NULL,",");
		i++;
	    }
	    return_flag2 = 1;
	}
    }
	///////////////여기서부터 앞의 dump명령어와 비슷하게 처리함///////////////////
    if(strstr(command,"edit") || command[0]=='e')
    {
	if(store2[0]=='e' && store2[1]=='d' && store2[2]=='i' && store2[3]=='t')
	{
	    i = check_store2("edit",store2);
	    if(return_flag2 == 2)//command=="edit"에서 끝날 때 인자가 없으니까 바로 -1 리턴
		return -1;
	}
	else if(store2[0]=='e')
	{
	    i = check_store2("e",store2);
	    if(return_flag2 == 2)//command=="e"에서 끝날 때 인자가 없으니까 바로 -1 리턴
		return -1;
	}
	else
	    return_flag2 = -1;/////////////이 과정에서 정상 명령어라고 가정했을 때 다음에 붙은 다른 문자가 있는지, 없고 공백만 있는지 확인한다.

    	strcpy(dumpRange,store2+i+1);//dump 명령어 이후의 인자만 dumpRange에 저장
	cut_space(dumpRange);//dumpRange 앞뒤 공백 있으면 자름

	for(i=0; (unsigned)i<strlen(dumpRange); i++)
	{
	    if(dumpRange[i] == ',')
		comma_index1=i;
	}
	if(comma_index1==-1) //콤마가 없으면;; edit 은 인자 두개니까 콤마없으면 바로 에러!
	    return_flag2=-1;
	else
	{
	    start_flag=0;
	    for(i=comma_index1-1; i>=0; i--)
	    {
		if(start_flag==0 && ((dumpRange[i] != ' ') && (dumpRange[i] != '\t')))
		    start_flag=1;
		if(start_flag==1)
		{
		    if((dumpRange[i]>='a' && dumpRange[i]<='f') || (dumpRange[i]>='A' && dumpRange[i]<='F') || (dumpRange[i]>='0' && dumpRange[i]<='9'))
			continue;
		    else
		    {
			return_flag2=-1;
			break;
		    }
		}
	    }////콤마 앞

	    start_flag=0;
	    for(i=comma_index1+1; (unsigned)i<strlen(dumpRange); i++)
	    {      
		if(start_flag==0 && ((dumpRange[i] != ' ') && (dumpRange[i] != '\t')))
		    start_flag=1;
		if(start_flag==1)
		{
		    if((dumpRange[i]>='a' && dumpRange[i]<='f') || (dumpRange[i]>='A' && dumpRange[i]<='F') || (dumpRange[i]>='0' && dumpRange[i]<='9'))
			continue;
		    else
		    {
			return_flag2=-1;
			break;
		    }
		}
	    }////콤마 뒤
	}

	if(comma_index1 != -1 && return_flag2 != -1)
	{
	    ptr = strtok(dumpRange,",");
	    i=1;
	    while(ptr!=NULL)
	    {
		if(i==1)
		    address = strtoul(ptr,NULL,16);
		else if(i==2)
		    value = strtoul(ptr,NULL,16);
		ptr = strtok(NULL,",");
		i++;
	    }
	    return_flag2 = 1;
	}
    }

    if(strstr(command,"fill") || command[0]=='f')
    {
	if(store2[0]=='f' && store2[1]=='i' && store2[2]=='l' && store2[3]=='l')
	{
	    i = check_store2("fill",store2);
	    if(return_flag2 == 2)//command=="fill"에서 끝날 때 인자가 없으므로 바로 -1 리턴
		return -1;
	}
	else if(store2[0]=='f')
	{
	    i = check_store2("f",store2);
	    if(return_flag2 == 2)//command=="f"에서 끝날 때 인자가 없으므로 바로 -1 리턴
		return -1;
	}
	else
	    return_flag2 = -1;/////////////이 과정에서 정상 명령어라고 가정했을 때 다음에 붙은 다른 문자가 있는지, 없고 공백만 있는지 확인한다.

    	strcpy(dumpRange,store2+i+1);//dump 명령어 이후의 인자만 dumpRange에 저장
	cut_space(dumpRange);//dumpRange 앞뒤 공백 있으면 자름
	
	for(i=0; (unsigned)i<strlen(dumpRange); i++)
	{
	    if(dumpRange[i] == ',')
	    {
		comma_index1=i;
		break;
	    }
	}//첫번째 콤마
	if(comma_index1 != -1)
	{
	    for(i=comma_index1+1; (unsigned)i<strlen(dumpRange); i++)
	    {
		if(dumpRange[i] == ',')
		    comma_index2=i;
	    }
	}//두번째 콤마
	if(comma_index1==-1 || comma_index2==-1) //콤마가 하나라도 없으면;; fill은 콤마 없으면 바로 에러!
	    return_flag2 = -1;
	else
	{
	    start_flag=0;
	    for(i=comma_index1-1; i>=0; i--)
	    {
		if(start_flag==0 && ((dumpRange[i] != ' ') && (dumpRange[i] != '\t')))
		    start_flag=1;
		if(start_flag==1)
		{
		    if((dumpRange[i]>='a' && dumpRange[i]<='f') || (dumpRange[i]>='A' && dumpRange[i]<='F') || (dumpRange[i]>='0' && dumpRange[i]<='9'))
			continue;
		    else
		    {
			return_flag2=-1;
			break;
		    }
		}
	    }////콤마 앞
	    
	    start_flag=0;
	    for(i=comma_index2-1; i>comma_index1; i--)
	    {
		if(start_flag==0 && ((dumpRange[i] != ' ') && (dumpRange[i] != '\t')))
		    start_flag=1;
		if(start_flag==1)
		{
		    if((dumpRange[i]>='a' && dumpRange[i]<='f') || (dumpRange[i]>='A' && dumpRange[i]<='F') || (dumpRange[i]>='0' && dumpRange[i]<='9'))
			continue;
		    else if((dumpRange[i] == ' ') || (dumpRange[i] == '\t'))
		    {
			i--;
			while(i>comma_index1)
			{
			    if((dumpRange[i]==' ') || (dumpRange[i]=='\t'))
				continue;
			    else
			    {
				return_flag2=-1;
				break;
			    }
			}
		    }
		    else
		    {
			return_flag2=-1;
			break;
		    }
		}
	    }////콤마 사이;;;fill은 인자가 3개, 콤마가 두개이므로 처리과정 추가

	    start_flag=0;
	    for(i=comma_index2+1; (unsigned)i<strlen(dumpRange); i++)
	    {      
		if(start_flag==0 && ((dumpRange[i] != ' ') && (dumpRange[i] != '\t')))
		    start_flag=1;
		if(start_flag==1)
		{
		    if((dumpRange[i]>='a' && dumpRange[i]<='f') || (dumpRange[i]>='A' && dumpRange[i]<='F') || (dumpRange[i]>='0' && dumpRange[i]<='9'))
			continue;
		    else
		    {
			return_flag2=-1;
			break;
		    }
		}
	    }////콤마 뒤
	}
	
	if(comma_index1 != -1 && comma_index2 != -1 && return_flag2 != -1)
	{
	    ptr = strtok(dumpRange,",");
	    i=1;
	    while(ptr!=NULL)
	    {
		if(i==1)
		    start = strtoul(ptr,NULL,16);
		else if(i==2)
		    end = strtoul(ptr,NULL,16);
		else if(i==3)
		    value = strtoul(ptr,NULL,16);
		ptr = strtok(NULL,",");
		i++;
	    }
	    return_flag2 = 1;
	}
    }
    
    if(strstr(command,"dir") || command[0]=='d')
    {
	if(store2[0]=='d' && store2[1]=='i' && store2[2]=='r')
	{
	    i = check_store2("dir",store2);
	    if(return_flag2 == 2)
		return 1;//dir만 치고 끝나면 바로 실행
	}
	else if(store2[0]=='d')
	{
	    for(i=0; (unsigned)i<strlen(store2); i++)
	    {
		if(store2[i]=='d' || store2[i]=='D')
		    break;
	    }
	    if((store2[i+1] != ' ') && (store2[i+1] != '\t') && store2[i+1] != '\0' && store2[i+1] != 'u')//dump의 d와 겹쳐서 따로 처리
		return_flag2=-1;
	    else if((store2[i+1] == ' ') || (store2[i+1] == '\t'))
	    {
		for(i=i+1; (unsigned)i<strlen(store2);i++)
		{
		    if((store2[i] != ' ') && (store[i] != '\t'))
			return -1;//이 명령어는 한글자로도 되기때문에(d) 그 뒤에 공백이 있는게 한글자라서인지 잘못된명령어인지 예외처리;;;뒤의 h,q등도 비슷
		}
	    }
	}
	else
	    return_flag2 = -1;
    }

    if(strstr(command,"history") || strstr(command,"hi"))
    {
	if(store2[0]=='h' && store2[1]=='i' && store2[2]=='s' && store2[3]=='t' && store2[4]=='o' && store2[5]=='r' && store2[6]=='y')
	{
	    i = check_store2("history",store2);
	    if(return_flag2 == 2)
		return 1;//history만 치고 끝나면 바로 실행
	}
	else if(store2[0]=='h' && store2[1]=='i')
	{
	    for(i=0; (unsigned)i<strlen(store2); i++)
	    {
		if(store2[i]=='i' || store2[i]=='I')
		    break;
	    }
	    if((store2[i+1] != ' ') && (store2[i+1] != '\t') && store2[i+1] != '\0')
		return_flag2=-1;
	    else if((store2[i+1] == ' ') || (store2[i+1] == '\t'))
	    {
		for(i=i+1; (unsigned)i<strlen(store2);i++)
		{
		    if((store2[i] != ' ') && (store[i] != '\t'))
			return -1;//이 명령어는 한글자로도 되기때문에(d) 그 뒤에 공백이 있는게 한글자라서인지 잘못된명령어인지 예외처리;;;뒤의 h,q등도 비슷
		}
	    }
	}
	else
	    return_flag2 = -1;
    }

    if(strstr(command,"help") || command[0]=='h')
    {
	if(store2[0]=='h' && store2[1]=='e' && store2[2]=='l' && store2[3]=='p')
	{
	    i = check_store2("help",store2);
	    if(return_flag2 == 2)
		return 1;//help만 치고 끝나면 바로 실행
	}
	else if(store2[0]=='h')
	{
	    for(i=0; (unsigned)i<strlen(store2); i++)
	    {
		if(store2[i]=='h' || store2[i]=='H')
		    break;
	    }
	    if((store2[i+1] != ' ') && (store2[i+1] != '\t') && store2[i+1] != '\0' && store2[i+1] != 'i')//hi와 겹치므로 따로 처리
		return_flag2=-1;
	    else if((store2[i+1] == ' ') || (store2[i+1] == '\t'))
	    {
		for(i=i+1; (unsigned)i<strlen(store2);i++)
		{
		    if((store2[i] != ' ') && (store[i] != '\t'))
			return -1;//이 명령어는 한글자로도 되기때문에(d) 그 뒤에 공백이 있는게 한글자라서인지 잘못된명령어인지 예외처리;;;뒤의 h,q등도 비슷
		}
	    }
	}
	else
	    return_flag2 = -1;
    }

    if(strstr(command,"quit") || command[0]=='q')
    {
	if(store2[0]=='q' && store2[1]=='u' && store2[2]=='i' && store2[3]=='t')
	{
	    i = check_store2("quit",store2);
	    if(return_flag2 == 2)
		return 1;//quit만 치고 끝나면 바로 실행
	}
	else if(store2[0]=='q')
	{
	    for(i=0; (unsigned)i<strlen(store2); i++)
	    {
		if(store2[i]=='q' || store2[i]=='Q')
		    break;
	    }
	    if((store2[i+1] != ' ') && (store2[i+1] != '\t') && store2[i+1] != '\0')
		return_flag2=-1;
	    else if((store2[i+1] == ' ') || (store2[i+1] == '\t'))
	    {
		for(i=i+1; (unsigned)i<strlen(store2);i++)
		{
		    if((store2[i] != ' ') && (store[i] != '\t'))
			return -1;//이 명령어는 한글자로도 되기때문에(d) 그 뒤에 공백이 있는게 한글자라서인지 잘못된명령어인지 예외처리;;;뒤의 h,q등도 비슷
		}
	    }
	}
	else
	    return_flag2 = -1;
    }

    if(strstr(command,"reset"))
    {
	if(store2[0]=='r' && store2[1]=='e' && store2[2]=='s' && store2[3]=='e' && store2[4]=='t')
	{
	    i = check_store2("reset",store2);
	    if(return_flag2 == 2)
		return 1;//reset만 치고 끝나면 바로 실행
	}
	else
	    return_flag2 = -1;
    }

    if(strstr(command,"opcode"))
    {
	if(!strcmp(store2,"opcodelist"))
	    return 1;//opcodelist라고만 입력이면 바로 리스트 실행되게 미리 처리
	if(store2[0]=='o' && store2[1]=='p' && store2[2]=='c' && store2[3]=='o' && store2[4]=='d' && store2[5]=='e')
	    i = check_store2("opcode",store2);
	else
	    return_flag2 = -1;

	strcpy(dumpRange,store2+i+1);//opcode 명령어 이후의 것만 dumpRange에 저장
	cut_space(dumpRange);//앞뒤공백 자름

	if(strlen(dumpRange)==0)
	    return_flag2 = -1;//opcode만 치고 인자를 안쳤을 때 바로 오류

	for(i=0; (unsigned)i<strlen(dumpRange); i++)
	{
	    if((dumpRange[i]>='A' && dumpRange[i]<='Z'))
		continue;
	    else
	    {
		return_flag2=-1;
		break;
	    }
	}
	if(return_flag2 != -1)
	{
	    strcpy(mnemonic,dumpRange);
	    return_flag2 = 1;
	}
	else
	    printf("error : 적절하지 않은 mnemonic입니다. (소문자, 숫자 등등)\n");
    }

    if(strstr(command,"type"))
    {
	if(store2[0]=='t' && store2[1]=='y' && store2[2]=='p' && store2[3]=='e')
	    i = check_store2("type",store2);
	else
	    return_flag2 = -1;

	strcpy(dumpRange,store2+i+1);//type 명령어 이후의 것만 dumpRange에 저장
	cut_space(dumpRange);//앞뒤공백 자름

	if(strlen(dumpRange)==0)
	    return_flag2 = -1;//type만 치고 인자를 안쳤을 때 바로 오류

	if(return_flag2 != -1)
	{
	    strcpy(fileName,dumpRange);
	    return_flag2 = 1;
	}
	else
	    printf("error : filename을 입력하세요.\n");
    }

    if(strstr(command,"progaddr"))
    {
	if(store2[0]=='p' && store2[1]=='r' && store2[2]=='o' && store2[3]=='g' && store2[4]=='a' && store2[5]=='d' && store2[6]=='d' && store2[7]=='r')
	    i = check_store2("progaddr",store2);
	else
	    return_flag2 = -1;

	strcpy(dumpRange,store2+i+1);//progaddr 명령어 이후의 것만 dumpRange에 저장
	cut_space(dumpRange);//앞뒤공백 자름

	if(strlen(dumpRange)==0)
	    return_flag2 = -1;//progaddr만 치고 인자를 안쳤을 때 바로 오류

	if(return_flag2 != -1)
	{
	    progaddr = strtoul(dumpRange,NULL,16);
	    return_flag2 = 1;
	}
	else
	    printf("error : 주소를 입력하세요.\n");
    }

    if(strstr(command,"assemble"))
    {
	if(store2[0]=='a' && store2[1]=='s' && store2[2]=='s' && store2[3]=='e' && store2[4]=='m' && store2[5]=='b' && store2[6]=='l' && store2[7]=='e')
	    i = check_store2("assemble",store2);
	else
	    return_flag2 = -1;

	strcpy(dumpRange,store2+i+1);//assemble 명령어 이후의 것만 dumpRange에 저장
	cut_space(dumpRange);//앞뒤공백 자름

	if(strlen(dumpRange)==0)
	    return_flag2 = -1;//assemble만 치고 인자를 안쳤을 때 바로 오류

	if(return_flag2 != -1)
	{
	    strcpy(fileName_assem,dumpRange);
	    return_flag2 = 1;
	}
	else
	    printf("error : 정확한 filename을 입력하세요.\n");
    }

    if(strstr(command,"loader"))
    {
	if(store2[0]=='l' && store2[1]=='o' && store2[2]=='a' && store2[3]=='d' && store2[4]=='e' && store2[5]=='r')
	    i = check_store2("loader",store2);
	else
	    return_flag2 = -1;
	
	strcpy(dumpRange,store2+i+1);//loader 명령어 이후의 것만 dumpRange에 저장
	cut_space(dumpRange);//앞뒤공백 자름

	if(strlen(dumpRange)==0)
	    return_flag2 = -1;//loader만 치고 인자를 안쳤을 때 바로 오류

	if(return_flag2 != -1)
	{
	    return_flag2 = 1;
	    i=0;
	    while((unsigned int)i<strlen(dumpRange))
	    {
		word_flag = 0;
		strcpy(store_obf,"\0");
		j=0;
		for(;(unsigned int)i<strlen(dumpRange); i++)
		{
		    if(dumpRange[i] == ' ' || dumpRange[i] == '\t')
		    {
			i++;
			break;
		    }
		    else
		    {
			word_flag = 1;
			store_obf[j] = dumpRange[i];
			j++;
		    }
		}

		if(word_flag == 1)
		{
		    filenum++;
		    if(filenum == 1)
			strcpy(obf1,store_obf);
		    else if(filenum == 2)
			strcpy(obf2,store_obf);
		    else if(filenum == 3)
		    strcpy(obf3,store_obf);
		    else if(filenum > 3)
		    {
			return_flag2 = -1;
			printf("error : load할 object 파일은 최대 3개입니다.\n");
		    }
		}
	    }
	}
	else
	    printf("error : filename을 입력하세요.\n");
    }

    if(strstr(command,"symbol"))
    {
	if(store2[0]=='s' && store2[1]=='y' && store2[2]=='m' && store2[3]=='b' && store2[4]=='o' && store2[5]=='l')
	{
	    i = check_store2("symbol",store2);
	    if(return_flag2 == 2)
		return 1;//reset만 치고 끝나면 바로 실행
	}
	else
	    return_flag2 = -1;
    }

    if(return_flag1 == -1 || return_flag2 == -1)
	return -1;//정상 명령어 아니고 예외처리되어있음
    else if(return_flag1==1 && return_flag2==1)
	return 1; //정상 명령어임
    else
	return 0;
}

Node* Create_Node(char* data)
{
    Node* newnode=(Node*)malloc(sizeof(Node));
    strcpy(newnode->data,data);
    newnode->link=NULL;

    return newnode;
}//노드 생성

Table* Create_Table(int key, char* name, char* format)
{
    Table* new = (Table*)malloc(sizeof(Table));
    new->id = key;
    strcpy(new->instruct,name);
    strcpy(new->format,format);
    new->hashNext=NULL;

    return new;
}//테이블에 필요한 노드 생성

void DumpEditFill(int start, int end, int address, int value)
{
    signed long i,j;
    signed long temp;
    signed long endTemp;
    unsigned int endLinenum;
    if(start!=-1 && end==-1 && address==-1 && value==-1 && start<=1048575)//dump start일때처리
    {
	insertBack(&head,&tail,Create_Node(store_command));
	temp = start%16;
	line_num = (unsigned int)(start-temp);
	dump_p = (unsigned int)start;
	for(i=0; i<10; i++)
	{
	    if(line_num > 1048560)
		break;//FFFFF보다 큰 수 들어올때 처리
	    printf("%05X ",line_num);
	    if(i==0)
	    {
		for(j=0; j<temp; j++)
		    printf("   ");
		for(j=temp; j<16; j++)
		{
		    printf("%02X ",virtual[dump_p]);
		    dump_p++;
		}
		printf("; ");
		dump_p = dump_p-(unsigned int)(16-temp);
		for(j=0; j<temp; j++)
		    printf(".");
		for(j=temp; j<16; j++)
		{
		    if((virtual[dump_p]>=0x20) && (virtual[dump_p]<=0x7E))
			printf("%c",virtual[dump_p]);
		    else
			printf(".");
		    dump_p++;
		}
		printf("\n");
	    }
	    else
	    {
		for(j=0; j<16; j++)
		{
		    printf("%02X ",virtual[dump_p]);
			dump_p++;
		}
		printf("; ");
		dump_p = dump_p - 16;
		for(j=0; j<16; j++)
		{
		    if((virtual[dump_p]>=0x20) && (virtual[dump_p]<=0x7E))
			printf("%c",virtual[dump_p]);
		    else
			printf(".");
		    dump_p++;
		}
		printf("\n");
	    }
	    line_num+=16;
	}
	if(line_num <= 1048560 && temp!=0)
	{
	    printf("%05X ",line_num);
	    for(i=0; i<temp; i++)
	    {
		printf("%02X ",virtual[dump_p]);
		dump_p++;
	    }
	    for(i=temp; i<16; i++)
		printf("   ");
	    printf("; ");
	    dump_p = dump_p - temp;
	    for(i=0; i<temp; i++)
	    {
		if((virtual[dump_p]>=0x20) && (virtual[dump_p]<=0x7E))
		    printf("%c",virtual[dump_p]);
		else
		    printf(".");
		dump_p++;
	    }
	    for(i=temp; i<16; i++)
		printf(".");
	    printf("\n");//160개 출력을 위한 처리
	}
    }
    else if(start!=-1 && end==-1 && address==-1 && value==-1 && start>1048575)
	printf("error : 0xFFFFF 보다 큰 값은 주소로 들어올 수 없습니다.\n");

    if(start!=-1 && end!=-1 && address==-1 && value==-1 && start<=1048575 && end<=1048575)//dump start end일때
    {
	if(start >= end)
	{
	    printf("error : start는 end값보다 크거나 같을  수 없습니다.\n");
	    return;
	}//start>end일때 에러처리
	insertBack(&head,&tail,Create_Node(store_command));
	temp = start%16;
	line_num = (unsigned int)(start-temp);
	endTemp = end%16;
	endLinenum = (unsigned int)(end-endTemp);
	dump_p = (unsigned int)start;
	if((line_num == endLinenum) && ((end-start)<16))//범위가 한 라인 이내일때
	{
	    printf("%05X ",line_num);
	    if(line_num == (unsigned int)(start-temp))
	    for(i=0; i<temp; i++)
		printf("   ");
	    for(i=temp; i<=endTemp; i++)
	    {
		printf("%02X ",virtual[dump_p]);
		dump_p++;
	    }
	    for(i=endTemp+1; i<16; i++)
		printf("   ");
	    printf("; ");
	    dump_p = dump_p - (unsigned int)(endTemp-temp+1);
	    for(i=0; i<temp; i++)
		printf(".");
	    for(i=temp; i<=endTemp; i++)
	    {
		if((virtual[dump_p]>=0x20) && (virtual[dump_p]<=0x7E))
		    printf("%c",virtual[dump_p]);
		else
		    printf(".");
		dump_p++;
	    }
	    for(i=endTemp+1; i<16; i++)
		printf(".");
	}
	while((line_num <= endLinenum) && ((end-start>=16)))//범위가 여러라인으로 넘어갈때
	{
	    if(line_num > 1048560)
		break;
	    printf("%05X ",line_num);
	    if(line_num == (unsigned int)(start-temp))
	    {
		for(i=0; i<temp; i++)
		    printf("   ");
		for(i=temp; i<16; i++)
		{
		    printf("%02X ",virtual[dump_p]);
		    dump_p++;
		}
		printf("; ");
		dump_p = dump_p - (unsigned int)(16-temp);
		for(i=0; i<temp; i++)
		    printf(".");
		for(i=temp; i<16; i++)
		{
		    if((virtual[dump_p]>=0x20) && (virtual[dump_p]<=0x7E))
			printf("%c",virtual[dump_p]);
		    else
			printf(".");
		    dump_p++;
		}
	    }//첫줄일때
	    else if(line_num == endLinenum)
	    {
		for(i=0; i<=endTemp; i++)
		{
		    printf("%02X ",virtual[dump_p]);
		    dump_p++;
		}
		for(i=(endTemp+1); i<16; i++)
		    printf("   ");
		printf("; ");
		dump_p = dump_p - (unsigned int)endTemp;
		for(i=0; i<=endTemp; i++)
		{
		    if((virtual[dump_p]>=0x20) && (virtual[dump_p]<=0x7E))
			printf("%c",virtual[dump_p]);
		    else
			printf(".");
		    dump_p++;
		}
		for(i=(endTemp+1); i<16; i++)
		    printf(".");
	    }//마지막줄일때
	    else
	    {
		for(i=0; i<16; i++)
		{
		    printf("%02X ",virtual[dump_p]);
		    dump_p++;
		}
		printf("; ");
		dump_p = dump_p - 16;
		for(i=0; i<16; i++)
		{
		    if((virtual[dump_p]>=0x20) && (virtual[dump_p]<=0x7E))
			printf("%c",virtual[dump_p]);
		    else
			printf(".");
		    dump_p++;
		}
	    }//나머지 가운데 줄일때
	    printf("\n");
	    line_num = line_num + 16;
	}
    }
    else if(start!=-1 && end!=-1 && address==-1 && value==-1 && (start>1048575 || end>1048575))
	printf("error : 0xFFFFF 보다 큰 값은 주소로 들어올 수 없습니다.\n");

    if(start==-1 && end==-1 && address!=-1 && value!=-1 && address<=1048575 && value<=255)//edit address value 일때
    {
	insertBack(&head,&tail,Create_Node(store_command));
	dump_p = (unsigned int)address;
	virtual[dump_p] = value;
    }
    else if(start==-1 && end==-1 && address!=-1 && value!=-1 && (address>1048575 || value>255))
	printf("error : 0xFFFFF 보다 큰 값은 주소로 들어올 수 없습니다. value값은 0xFF보다 클 수 없습니다.\n");

    if(start!=-1 && end!=-1 && address==-1 && value!=-1 && start<=1048575 && end<=1048575 && value<=255)//fill start end value일때
    {
	insertBack(&head,&tail,Create_Node(store_command));
	temp_p = dump_p;//임시저장
	dump_p = (unsigned int)start;
	while(dump_p <= (unsigned int)end)
	{
	    virtual[dump_p] = value;
	    dump_p++;
	}
	dump_p = temp_p;//원상복구
    }
    else if(start!=-1 && end!= -1 && address==-1 && value!=-1 && (start>1048575 || end>1048575 || value>255))
	printf("error : 0xFFFFF 보다 큰 값은 주소로 들어올 수 없습니다. value값은 0xFF보다 클 수 없습니다.\n");
}

int hashFunc(char* mnemonic)
{
    unsigned int hashkey=30;
    unsigned int key;
    char name[MAX];
    char format[MAX];
    FILE *fp;
    fp=fopen("opcode.txt","r");
    while(1)
    {
	if(fscanf(fp,"%X %s %s\n",&key,name,format)==EOF)
	    break;
	else
	{
	    if(!strcmp(mnemonic,name))
		hashkey = HASH_KEY(key);//매크로에 있음
	}
    }
    fclose(fp);
    return hashkey;
}//해시함수 ;; 명령어 mnemonic을 받아서 그 명령어의 값을 함수를 통해 키값으로 변환하여  반환해줌.

int makeLST(char* fileName_assem)
{
    int LSTline;
    unsigned long loc = 0;
    unsigned int first_loc, last_loc, last_loc_real;
    unsigned int opcode;
    unsigned int id;
    FILE *fp_ASM;
    FILE *fp_LST_pass1;
    FILE *fp_LST_pass2;
    FILE *fp_OBJ;
    int isComment;
    int i, j;
    int symTab_flag;
    int format1_flag;
    int format2_flag;
    int format4_flag;
    int immediate_flag;
    int number_immediate_flag;
    int index_flag;
    int find_flag;
    char format[10] = {'\0',};
    char storeAddr[15] = {'\0',};
    char storeOperand[35] = {'\0',};
    char storeTR[35] = {'\0',};
    char storeTR2[MAX];
    unsigned int TR_flag, TR_start, TR_end, tempLoc;
    int stopFlag;
    Table* new = NULL;
    strcpy(fileName_assem_store,fileName_assem);
    strcat(fileName_assem,".lst");
    strcat(fileName_assem_store,".obj");

    fp_ASM = fopen(fileName,"r");
    fp_LST_pass1 = fopen("temp.txt","w");

    symTab_i = 0;
    loc = 0;
    LSTline = 0;
    storeLoc_i = 0;
    while(fgets(fileStr, MAX-1, fp_ASM) != NULL)
    {
	LSTline += 5;
	immediate_flag = 0;
	index_flag = 0;
	format1_flag = 0;
	format4_flag = 0;
	format2_flag = 0;

	fileStr[strlen(fileStr)-1] = '\0';//개행 없애고

	for(i = 0; i<MAX; i++)
	{
	    assem.symbol[i] = '\0';
	    assem.mnemonic[i] = '\0';
	    assem.operand[i] = '\0';
	}
	isComment = findSpace(fileStr,1);

	if(isComment == 0)//주석이다..
	{
	    fprintf(fp_LST_pass1,"%s\n",fileStr);
	    continue;
	}
	else if(isComment == 1)//정상 소스파일이고 받은 문자열은 주석이 아님
	{
	    if(assem.mnemonic[0] == '+')
	    {
		format4_flag = 1;
		strcpy(assem.mnemonic, assem.mnemonic+1);
	    }

	    if(assem.operand[0] == '#')
	    {
		immediate_flag = 1;
		strcpy(assem.operand, assem.operand+1);
	    }

	    if(assem.operand[0] == '@')
	    {
		strcpy(assem.operand, assem.operand+1);
	    }

	    if(!strcmp(assem.mnemonic,"START"))
	    {
		loc += strtoul(assem.operand,NULL,16);
		fprintf(fp_LST_pass1,"%04X   %s\n",(unsigned int)loc,fileStr);
	//	loc += strtoul(assem.operand,NULL,10);
	    }
	    else if(!strcmp(assem.mnemonic,"END"))
		fprintf(fp_LST_pass1,"%s\n",fileStr);
	    else
	    {
		if(!strcmp(assem.mnemonic,"BASE"))
		    fprintf(fp_LST_pass1,"%s\n",fileStr);
		else
		    fprintf(fp_LST_pass1,"%04X   %s\n",(unsigned int)loc,fileStr);

		if(strlen(assem.symbol) != 0)
		{
		 //   symTab_i++;
		    symTab_flag = 0;
		    for(i=0; i< symTab_i; i++)
		    {
			if(!strcmp(symTab[i].symbol,assem.symbol))
			{
			    symTab_flag = 1;
			    break;
			}
		    }

		    if(symTab_flag != 1)//없으면 추가
		    {
			symTab[symTab_i].loc = loc;
			strcpy(symTab[symTab_i].symbol, assem.symbol);
			symTab_i++;
		    }
		}//symbol table 만들기

		id = hashFunc(assem.mnemonic);
		if(id<MAX_HASH)
		{
		    if(!strcmp(hashTable[id]->instruct,assem.mnemonic))
		    {
			opcode = hashTable[id]->id;
			strcpy(format,hashTable[id]->format);
		    }
		    else
		    {
			new = hashTable[id];
			while(new->hashNext)
			{
			    if(!strcmp(new->hashNext->instruct,assem.mnemonic))
			    {
				opcode = new->hashNext->id;
				strcpy(format,new->hashNext->format);
			    }
			    new = new->hashNext;
			}
		    }

		    if(format4_flag==1)
		    {
			loc += 4;
		    }
		    else if(!strcmp(format,"2"))
		    {
			format2_flag = 1;
			loc += 2;
		    }
		    else if(!strcmp(format,"1"))
		    {
			format1_flag = 1;
			loc += 1;
		    }
		    else
			loc += 3;
		}//opcode찾아서 있으면 loc+3, format4면 +4
		else if(!strcmp(assem.mnemonic,"RESB"))
		    loc += strtoul(assem.operand,NULL,10);
		else if(!strcmp(assem.mnemonic,"RESW"))
		    loc += (3 * (strtoul(assem.operand,NULL,10)));
		else if(!strcmp(assem.mnemonic,"WORD"))
		    loc += 3;
		else if(!strcmp(assem.mnemonic,"BYTE"))
		{
		    for(i=0; (unsigned int)i<strlen(assem.operand); i++)
		    {
			if(assem.operand[i] != '\'')
			    continue;
			else if(assem.operand[i] == '\'')
			    break;
		    }
		    for(i = i+1; (unsigned int)i<strlen(assem.operand); i++)
		    {
			if(assem.operand[i] != '\'')
			{
			    if(assem.operand[0] == 'C')
				loc++;
			    else if(assem.operand[0] == 'X')
			    {
				loc++;
				i++;
			    }
			}
			else if(assem.operand[i] == '\'')
			    break;
		    }
		}
		else
		{
		    if(format4_flag != 0)
		    {
			printf("error line %d : 명령어가 opcode table에 없습니다. 잘못된 asm 소스코드 입니다.\n",LSTline);
			return -1;
		    }
		}//변수 상수도 아니고 optable에 없음->에러
	    }
	    
	    if(!strcmp(assem.mnemonic,"RESW") || !strcmp(assem.mnemonic,"RESB"))
		storeLoc[storeLoc_i].RES_flag = 1;
	    else
		storeLoc[storeLoc_i].RES_flag = 0;

	    if(strcmp(assem.mnemonic,"BASE") && strcmp(assem.mnemonic,"END"))
	    {
		storeLoc[storeLoc_i].loc = loc;
		storeLoc_i++;
	    }
	}
	else
	{
	    printf("error line %d : 정상 소스파일이 아닙니다.\n",LSTline);//정상 소스파일 아닐 때
	    return -1;
	}
    }

    first_loc = storeLoc[0].loc;
    storeLoc_i -= 1;
    last_loc_real = storeLoc[storeLoc_i].loc;
    storeLoc_i -= 1;
    last_loc = storeLoc[storeLoc_i].loc;
    fclose(fp_ASM);
    fclose(fp_LST_pass1);////////////////pass1 끝;; END와 BASE 지시어를 뺀 구간에서 loc 지정 / end줄의 마지막 loc은 last_loc_real에 있고 이는 출력하지는않는다


    ///////////////pass2시작////////////////////////
    fp_ASM = fopen("temp.txt","r");
    fp_LST_pass2 = fopen(fileName_assem,"w");
    fp_OBJ = fopen(fileName_assem_store,"w");
    LSTline = 0;
    TR_flag = 1;
    stopFlag = 0;
    mod_i = 0;
    TR_start = TR_end = first_loc;

    while(fgets(fileStr, MAX-1, fp_ASM) != NULL)
    {
	find_flag = 0;
	format1_flag = 0;
	format2_flag = 0;
	format4_flag = 0;
	immediate_flag = 0;
	number_immediate_flag = 0;
	index_flag = 0;
	LSTline += 5;
	fprintf(fp_LST_pass2, "%8d     ", LSTline);
	obj.ni = 3;//대체로 심플어드레싱
	obj.x = obj.b = obj.p = obj.e = 0;
	obj.xbpe = obj.addr = obj.toSub = 0;

	fileStr[strlen(fileStr)-1] = '\0';//개행 없애고

	for(i = 0; i<MAX; i++)
	{
	    assem.symbol[i] = '\0';
	    assem.mnemonic[i] = '\0';
	    assem.operand[i] = '\0';
	}
	isComment = findSpace(fileStr,2);

	if(isComment == 0)//주석이다..
	{
	    fprintf(fp_LST_pass2,"                    %s\n",fileStr);
	    continue;
	}
	else if(isComment == 1)//정상 소스파일이고 받은 문자열은 주석이 아님
	{
	    if(!strcmp(assem.mnemonic,"END") || !strcmp(assem.mnemonic,"BASE"))
		fprintf(fp_LST_pass2,"         %-10s %-10s %-10s",assem.symbol,assem.mnemonic,assem.operand);
	    else
		fprintf(fp_LST_pass2,"%04X     %-10s %-10s %-10s               ",assem.loc,assem.symbol,assem.mnemonic,assem.operand);
	    pc_relative = assem.loc + 3;//대체로 3차이

	    j=0;
	    for(i=0; (unsigned int)i<strlen(assem.operand); i++)
	    {
		if(assem.operand[i] == ' ' || assem.operand[i] == '\t')
		    continue;
		else
		{
		    storeOperand[j] = assem.operand[i];
		    j++;
		}
	    }
	    storeOperand[j] = '\0';
	    strcpy(assem.operand,storeOperand);

	    if(strstr(assem.operand,",X"))
	    {
		index_flag = 1;
		obj.x = 1;
		for(j=0; assem.operand[j] != ','; j++)
		    storeOperand[j] = assem.operand[j];
		storeOperand[j] = '\0';
		strcpy(assem.operand,storeOperand);
	    }

	    if(assem.mnemonic[0] == '+')
	    {
		format4_flag = 1;
		strcpy(assem.mnemonic, assem.mnemonic+1);
		obj.e = 1;
		pc_relative = assem.loc + 4;
	    }

	    if(assem.operand[0] == '#')
	    {
		immediate_flag = 1;
		strcpy(assem.operand, assem.operand+1);
		obj.ni = 1;//immediate
		pc_relative = assem.loc + 3;
	    }

	    if(assem.operand[0] == '@')
	    {
		strcpy(assem.operand, assem.operand+1);
		obj.ni = 2;//indirect
		pc_relative = assem.loc + 3;
	    }

	    if(!strcmp(assem.mnemonic,"RESW") || !strcmp(assem.mnemonic,"RESB"))
		stopFlag = 1;

	    if(stopFlag == 1 && ((strcmp(assem.mnemonic,"RESW") && strcmp(assem.mnemonic,"RESB"))))
		stopFlag = 2;

	    if(strlen(TR) > 60)
		TR_flag = 0;

	    if(LSTline == 5)//첫줄 obj file에 입력
		fprintf(fp_OBJ,"H%-6s%06X%06X\n",assem.symbol,(unsigned int)strtoul(assem.operand,NULL,16),(last_loc_real - first_loc));

	    if((TR_start <= last_loc) && (TR_flag == 0 || stopFlag == 2 || !strcmp(assem.mnemonic,"END")))
	    {
		if(TR_flag == 0)
		{
		    for(i = 0; i<MAX; i++)
		    {
			if(TR[i] == storeTR2[i])
			    continue;
			else
			    break;
		    }
		    
		    fprintf(fp_OBJ,"T%06X%02X%s\n",TR_start,(unsigned int)strlen(storeTR2)/2,storeTR2);

		    j=0;
		    for(; TR[i] != '\0'; i++)
		    {
			storeTR2[j] = TR[i];
			j++;
		    }
		    storeTR2[j] = '\0';
		    strcpy(TR,storeTR2);
		    TR_start = tempLoc;
		}
		else if(stopFlag == 2)
		{
		    fprintf(fp_OBJ,"T%06X%02X%s\n",TR_start,(unsigned int)strlen(TR)/2,TR);
		    strcpy(TR,"\0");
		    TR_start = assem.loc;
		}
		else if(!strcmp(assem.mnemonic,"END"))
		    fprintf(fp_OBJ,"T%06X%02X%s\n",TR_start,(unsigned int)strlen(TR)/2,TR);
		stopFlag = 0;
		TR_flag = 1;
		strcpy(storeTR,"\0");
		strcpy(storeTR2,"\0");
	    }//TR(text record)에 object code가 저장되어있음. 명령어가 end로 끝났거나 길이가 60자가 넘거나 변수일때마다 출력&개행

	    strcpy(storeTR2,TR);
	    tempLoc = assem.loc;

	    if(!strcmp(assem.mnemonic,"START"))
		fprintf(fp_LST_pass2,"\n");
	    else if(!strcmp(assem.mnemonic,"RESW") || !strcmp(assem.mnemonic,"RESB"))
		fprintf(fp_LST_pass2,"\n");
	    else if(!strcmp(assem.mnemonic,"BYTE"))
	    {
//		fprintf(fp_LST_pass2,"%-40s",fileStr);

		for(i=0; (unsigned int)i<strlen(assem.operand); i++)
		{
		    if(assem.operand[i] != '\'')
			continue;
		    else if(assem.operand[i] == '\'')
			break;
		}

		j=0;
		strcpy(storeTR,"\0");
		for(i = i+1; (unsigned int)i<strlen(assem.operand); i++)
		{
		    if(assem.operand[i] != '\'')
		    {
			if(assem.operand[0] == 'C')
			{
			    fprintf(fp_LST_pass2,"%02X",(unsigned int)assem.operand[i]);
			    sprintf(storeTR+j,"%02X",(unsigned int)assem.operand[i]);
			    j += 2;
			}
			else if(assem.operand[0] == 'X')
			{
			    fprintf(fp_LST_pass2,"%c",assem.operand[i]);
			    storeTR[j] = (unsigned int)assem.operand[i];
			    j++;
			}
		    }
		    else if(assem.operand[i] == '\'')
		    {
			fprintf(fp_LST_pass2,"\n");
			break;
		    }
		}
		storeTR[j] = '\0';
		strcat(TR,storeTR);
	    }
	    else if(!strcmp(assem.mnemonic,"WORD"))
	    {
//		fprintf(fp_LST_pass2,"%-40s",fileStr);
		fprintf(fp_LST_pass2,"%06X\n",(unsigned int)strtoul(assem.operand,NULL,10));
		strcat(TR,assem.operand);
	    }
	    else if(!strcmp(assem.mnemonic,"BASE"))
	    {
		fprintf(fp_LST_pass2,"\n");
		i=0;
		while(1)
		{
		    if(strlen(symTab[i].symbol) == 0)
			break;

		    if(!strcmp(symTab[i].symbol,assem.operand))
			base_relative = symTab[i].loc;
		    else if(immediate_flag == 1)
			base_relative = strtoul(assem.operand,NULL,10);
		    i++;
		}
	    }
	    else if(!strcmp(assem.mnemonic,"END"))
		fprintf(fp_LST_pass2,"\n");
	    else
	    {
		id = hashFunc(assem.mnemonic);
		if(id<MAX_HASH)
		{
		    if(!strcmp(hashTable[id]->instruct,assem.mnemonic))
		    {
			opcode = hashTable[id]->id;
			strcpy(format,hashTable[id]->format);
		    }
		    else
		    {
			new = hashTable[id];
			while(new->hashNext)
			{
			    if(!strcmp(new->hashNext->instruct,assem.mnemonic))
			    {
				opcode = new->hashNext->id;
				strcpy(format,new->hashNext->format);
			    }
			    new = new->hashNext;
			}
		    }
		    
		    i = 0;
		    while(1)
		    {
			if(strlen(symTab[i].symbol) == 0)
			    break;

			if(!strcmp(symTab[i].symbol,assem.operand))
			{
			    obj.addr = symTab[i].loc;
			    find_flag = 1;
			    break;
			}
			
			i++;
		    }//symtab에서 해당 operand이름을 찾아 loc 받아서 addr에 우선 저장
		    
		    if(find_flag == 0 && immediate_flag == 1)
		    {
			obj.addr = strtoul(assem.operand,NULL,10);
			find_flag = 1;
			obj.xbpe = 0;
			number_immediate_flag = 1;
		    }
		    
		    if(find_flag == 0 && find_reg_num(assem.operand) != -1)
			find_flag = 1;

		    if(find_flag == 0 && strlen(assem.operand) != 0)
		    {
			printf("error line %d : operand가 Symbol Table에 없습니다. 잘못된 asm 소스코드 입니다.\n",LSTline);
			return -1;
		    }

		    if(!strcmp(format,"4"))
		    {
			format4_flag = 1;
			obj.e = 1;
			pc_relative = assem.loc + 4;
		    }
		    else if(!strcmp(format,"2"))
		    {
			pc_relative = assem.loc + 2;
			format2_flag = 1;
		    }
		    else if(!strcmp(format,"1"))
		    {
			pc_relative = assem.loc + 1;
			format1_flag = 1;
		    }

		    if(number_immediate_flag != 1)
		    {
			obj.toSub = pc_relative;
			if(((signed int)obj.addr -(signed int)obj.toSub) < -2048 || ((signed int)obj.addr - (signed int)obj.toSub) > 2047)//이때는 pc로 안되어서 base로 전환
			{
			    obj.toSub = base_relative;
			    obj.b = 1;
			}
			else
			    obj.p = 1;
		    }//pc인지 base 인지 결정;;pc로 안되면 base 이용

		    if(format4_flag == 1)
		    {
			if(assem.mnemonic[0]=='J')
			{
			    obj.opcode = opcode + obj.ni;
			    obj.xbpe = 1;
			    obj.addr = obj.addr;
			}//점프이면 바로 로드
			else
			{
			    obj.opcode = opcode + obj.ni;
			    obj.xbpe = ((2*2*2*obj.x) + (2*2*obj.b) + (2*obj.p) + obj.e);
			    obj.addr = obj.addr - obj.toSub;
			}
		    }
		    else if(format2_flag == 1)
		    {
			obj.opcode = opcode;
			if(strstr(assem.operand,",") && (index_flag != 1))
			{
			    storeOperand[0] = assem.operand[0];
			    storeOperand[1] = '\0';
			    obj.reg1 = (unsigned int)find_reg_num(storeOperand);
			    for(j=0; (unsigned int)j<strlen(assem.operand); j++)
			    {
				if(assem.operand[j] == ',')
				    break;
			    }
			    for(j =j+1; (unsigned int)j<strlen(assem.operand); j++)
			    {
				if(assem.operand[j] != ' ' && assem.operand[j] != '\t')
				{
				    storeOperand[0] = assem.operand[j];
				    storeOperand[1] = '\0';
				    obj.reg2 = (unsigned int)find_reg_num(storeOperand);
				}
			    }

			}//인자가 두개일때
			else if(!strstr(assem.operand,","))//인자가 하나일때
			{
			    storeOperand[0] = assem.operand[0];
			    storeOperand[1] = '\0';
			    obj.reg1 = (unsigned int)find_reg_num(storeOperand);
			    obj.reg2 = 0;
			}

			if(obj.reg1 == 7 || obj.reg2 == 7 || obj.reg1 > 9 || obj.reg2 > 9)
			{
			    printf("error line %d : 잘못된 레지스터입니다. 알맞은 asm 소스코드가 아닙니다.\n",LSTline);
			    return -1;
			}
		    }
		    else if(format1_flag == 1)
			obj.opcode = opcode;
		    else if(!strcmp(assem.mnemonic,"RSUB"))
		    {
			obj.opcode = opcode + obj.ni;
			obj.xbpe = 0;
			obj.addr = 0;
		    }//modification
		    else//format3 일때
		    {
			obj.opcode = opcode + obj.ni;
			obj.xbpe = ((2*2*2*obj.x) + (2*2*obj.b) + (2*obj.p) + obj.e);
			obj.addr = obj.addr - obj.toSub;
		    }
		}
		else
		{
		    printf("error line %d : 명령어가 opcode table에 없습니다. 잘못된 asm 소스코드 입니다.\n",LSTline);
		    return -1;
		    //에러메세지와 리턴
		}
		
	//	fprintf(fp_LST_pass2,"%-40s",fileStr);
		strcpy(storeTR,"\0");
		if(format4_flag == 1)
		{
		    fprintf(fp_LST_pass2,"%02X%01X%05X\n",obj.opcode,obj.xbpe,obj.addr);//format3일 때
		    sprintf(storeTR,"%02X",obj.opcode);
		    strcat(TR,storeTR);
		    sprintf(storeTR,"%01X",obj.xbpe);
		    strcat(TR,storeTR);
		    sprintf(storeTR,"%05X",obj.addr);
		    strcat(TR,storeTR);
		}
		else if(format2_flag == 1)
		{
		    fprintf(fp_LST_pass2,"%02X%01X%01X\n",obj.opcode,obj.reg1,obj.reg2);
		    sprintf(storeTR,"%02X",obj.opcode);
		    strcat(TR,storeTR);
		    sprintf(storeTR,"%01X",obj.reg1);
		    strcat(TR,storeTR);
		    sprintf(storeTR,"%01X",obj.reg2);
		    strcat(TR,storeTR);
		}
		else if(format1_flag == 1)
		{
		    fprintf(fp_LST_pass2,"%02X\n",obj.opcode);
		    sprintf(storeTR,"%02X",obj.opcode);
		    strcat(TR,storeTR);
		}
		else//format3일때
		{
		    if((signed int)obj.addr < 0)
		    {
			sprintf(storeAddr,"%X",obj.addr);
			j=0;
			for(i=strlen(storeAddr)-3; (unsigned int)i<strlen(storeAddr); i++)
			{
			    storeAddr[j] = storeAddr[i];
			    j++;
			}
			storeAddr[j] = '\0';
			obj.addr = strtoul(storeAddr,NULL,16);
		    }

		    fprintf(fp_LST_pass2,"%02X%01X%03X\n",obj.opcode,obj.xbpe,obj.addr);
		    sprintf(storeTR,"%02X",obj.opcode);
		    strcat(TR,storeTR);
		    sprintf(storeTR,"%01X",obj.xbpe);
		    strcat(TR,storeTR);
		    sprintf(storeTR,"%03X",obj.addr);
		    strcat(TR,storeTR);
		}
	    }
	}

	if(!strcmp(assem.mnemonic,"JSUB"))
	{
	    if(format4_flag == 1)
	    {
		mod[mod_i].loc = assem.loc+1;
		mod[mod_i].val = 5;
		mod_i++;
	    }
	    else
	    {
		mod[mod_i].loc = assem.loc+1;
		mod[mod_i].val = 3;
		mod_i++;
	    }
	}
    }//modification record 저장

    for(i=0; i<mod_i; i++)
	fprintf(fp_OBJ,"M%06X%02X\n",mod[i].loc,mod[i].val);

    if(!strcmp(assem.mnemonic,"END"))
	fprintf(fp_OBJ,"E%06X\n",first_loc);
    fclose(fp_ASM);
    fclose(fp_LST_pass2);
    fclose(fp_OBJ);

    return 0;
}

int pass1_load(char* obf)
{//입력 파일에서 H,D레코드를 읽어 estab 생성
    FILE *fp;
    int pass1_return = 1;
    char record;
    char Name[MAX];
    char text[MAX];
    unsigned int addr;
    unsigned int len;
    int i;

    csaddr = progaddr;
    fp = fopen(obf,"r");
    while(1)
    {
	if(fscanf(fp,"%c",&record) == EOF)
	    break;
	else
	{
	    if(record == 'H')
	    {
		fscanf(fp,"%s %06X %06X",Name,&addr,&len);
		fgetc(fp);
		progaddr += addr + len;//다음 프로그램을 위한 progaddr 재설정
		for(i=0; i<=estab_i; i++)
		{
		    if(strcmp(estab[i].csname,Name))
			continue;
		    else
		    {
			printf("error : control section이 이미 estab에 있습니다.\n");
			pass1_return = -1;
			break;
		    }
		}

		if(pass1_return != -1)
		{//estab에 없다면 추가
		    strcpy(estab[estab_i].csname,Name);
		    estab[estab_i].addr = csaddr + addr;
		    estab[estab_i].len = len;
		    estab_i++;
		}
	    }//첫줄일때
	    
	    else if(record == 'D')
	    {
		while(1)
		{
		    fscanf(fp,"%s %06X",Name,&addr);

		    for(i=0; i<=estab_i; i++)
		    {
			if(strcmp(estab[i].symname,Name))
			    continue;
			else
			{
			    printf("error : symbol name이 이미 estab에 있습니다.\n");
			    pass1_return = -1;
			    break;
			}
		    }

		    if(pass1_return != -1)
		    {//estab 에 없다면 추가
			strcpy(estab[estab_i].symname,Name);
			estab[estab_i].addr = csaddr + addr;
			estab[estab_i].len = '\0';
			estab_i++;
		    }
		    
		    if(fgetc(fp) == '\n')
			break;//다음이 개행이면 루프 중지
		    else
			fseek(fp,-1,SEEK_CUR);//개행 아니면 파일포인터 원위치
		}		
	    }
	    else if(record=='R' || record=='T' || record=='M' || record=='.')
		fgets(text,MAX-1,fp);
	    else if(record == 'E')
		break;
	    else
	    {
		printf("error! : 알맞은 record가 아닙니다.\n");
		pass1_return = -1;
	    }
	}
    }
    
    fclose(fp);
    return pass1_return;
}

int pass2_load(char* obf)
{//pass1에서 생성한 estab을 통해 t,m레코드를 읽어 링크,메모리에 로드
    int pass2_return = 1;
    FILE* fp;
    int i,j,k,temp_i;
    int find_flag;//estab에서 찾았는지 여부
    int half_flag;//반비트 사용하는지 여부
    int find_ref = 0;//외부참조 찾았는지 여부
    char M_ch;//modify할때 더할지  뺄지 저장
    char tmp_ch;//half bit 가져올때 버리는 앞비트 저장
    unsigned int T_len, M_len;
    unsigned int T_addr, M_addr, fin_addr;
    char record;
    char exname[MAX];
    char T_text[MAX], M_text[MAX], mod_addr[MAX];
    char prog_name[MAX];
    char text[MAX];
    unsigned int prog_start, prog_len;
    unsigned int temp_addr;
    unsigned int mod_addr_hex, s_len;
    char put[3];
    unsigned int put_num[MAX],store_addr[MAX];

    fp = fopen(obf,"r");

    while(1)
    {
	if(fscanf(fp,"%c",&record) == EOF)
	    break;
	else
	{
	    if(record == 'H')
	    {
		fscanf(fp,"%s %06X %06X",prog_name,&prog_start,&prog_len);
		fgetc(fp);
		extref[0].refnum = 1;
		strcpy(extref[0].exname,prog_name);
		for(i=0; i<=estab_i; i++)
		{
		    if(!strcmp(estab[i].csname,prog_name))
			prog_start = estab[i].addr;
		}
	    }
	    else if(record == 'R')
	    {
		extref_i = 1;
		while(1)
		{
		    fscanf(fp,"%02X %s",&extref[extref_i].refnum,exname);
		    strcpy(extref[extref_i].exname,exname);
    
		    if(fgetc(fp) == '\n')
			break;//다음이 개행이면 루프 중지
		    else
		    {
			extref_i++;
			fseek(fp,-1,SEEK_CUR);//개행 아니면 파일포인터 원위치
		    }
		}
	    }
	    else if(record == 'T')
	    {
		fscanf(fp,"%06X %02X %s",&T_addr,&T_len,T_text);
		fgetc(fp);
		execaddr = prog_start + T_addr;

		j=0;
		for(i=0; i<MAX; i++)
		    put_num[i] = 0;

		for(i=0; (unsigned int)i<strlen(T_text); i+=2)
		{
		    put[0] = T_text[i];
		    put[1] = T_text[i+1];
		    put[2] = '\0';

		    put_num[j] = strtoul(put,NULL,16);
		    j++;
		}
		
		i=0;
		temp_addr = dump_p;//임시저장
		dump_p = execaddr;
		while(dump_p < execaddr+T_len)
		{	
		    virtual[dump_p] = put_num[i];
		    dump_p++;
		    i++;
		}
		dump_p = temp_addr;//원상복구
	    }//텍스트 레코드 가상 메모리에 올리기;; 중간에 16진수와 문자열 사이의 변환이 계속 있었다. (가상 메모리를 char로 선언)
	    else if(record == 'M')
	    {
		fscanf(fp,"%06X %02X %s",&M_addr,&M_len,M_text);
		fgetc(fp);
		M_ch = M_text[0];
		strcpy(M_text,M_text+1);
		for(i=0; i<MAX; i++)
		    store_addr[i] = 0;

		for(i=0; i<=extref_i; i++)
		{
		    if(strtoul(M_text,NULL,16) == extref[i].refnum)
		    {
			strcpy(M_text,extref[i].exname);
			find_ref = 1;
		    }
		    else
			continue;
		}
		
		if(find_ref != 1)
		{
		    printf("error! : 참조 내용이 estab에 없습니다.\n");
		    pass2_return = -1;
		    return pass2_return;//외부 참조의 내용이 estab에 없으면 error
		}

		execaddr = prog_start + M_addr;

		for(i=0; i<=estab_i; i++)
		{
		    find_flag = 0;
		    if(!strcmp(estab[i].symname,M_text) || !strcmp(estab[i].csname,M_text))
		    {
			temp_i = i;//나중에 해당 주소찾을때 쓰기위함
			find_flag = 1;
			half_flag = 0;
			k=0;
			if(M_len % 2 != 0)
			{
			    M_len++;
			    half_flag = 1;
			}
			M_len = M_len/2;

			for(j=execaddr;(unsigned int)j<execaddr+M_len; j++)
			{
			    store_addr[k] = virtual[j];
			    k++;
			}//우선 t레코드 값 읽어옴
			
			j=0;
			for(i=0; i<k; i++)
			{
			    sprintf(mod_addr+j,"%02X",store_addr[i]);
			    j+=2;
			}

			if(half_flag == 1)
			{
			    tmp_ch = mod_addr[0];
			    mod_addr[0] = '0';
			}//relocation을 할 때는 half bit를 이용해야 하는 경우가 있다. 이 경우 한 byte에서 앞 비트를 0으로 만들어줬다가 나중에 다시 원상복귀 시켜준다


			mod_addr_hex = strtoul(mod_addr,NULL,16);

			if(M_ch == '+')
			    fin_addr = mod_addr_hex + estab[temp_i].addr;
			else if(M_ch == '-')
			    fin_addr = mod_addr_hex - estab[temp_i].addr;

			sprintf(mod_addr,"%06X",fin_addr);
			if(half_flag == 1)
			    mod_addr[0] = tmp_ch;
			if(strlen(mod_addr) > M_len*2)
			{
			    s_len = strlen(mod_addr);
			    s_len = s_len - (M_len*2);
			    strcpy(mod_addr, mod_addr+s_len);//계산 결과가 원래의 길이보다 길어질때 그만큼 잘라준다
			}

			break;
		    }
		    else
			continue;
		}

		if(find_flag == 0)//심볼이 estab에 없을때
		{
		    printf("error : modify할 심볼이 estab에 없습니다.\n");
		    pass2_return = -1;
		}
		
		if(pass2_return != -1)
		{
		    for(i=0; i<MAX; i++)
			put_num[i] = 0;
		    j=0;

		    for(i=0; (unsigned int)i<strlen(mod_addr); i+=2)
		    {
			put[0] = mod_addr[i];
			put[1] = mod_addr[i+1];
			put[2] = '\0';

			put_num[j] = strtoul(put,NULL,16);
			j++;
		    }

		    i = 0;
		    temp_addr = dump_p;//임시저장
		    dump_p = execaddr;
		    while(dump_p < execaddr+M_len)
		    {	
			virtual[dump_p] = put_num[i];
			i++;
			dump_p++;
		    }
		    dump_p = temp_addr;//원상복구
		}//dump메모리에 t와 m레코드를 계산하여 로드한다
	    }
	    else if(record == 'D' || record == '.')
		fgets(text,MAX-1,fp);
	    else if(record == 'E')
		break;
	    else
	    {
		printf("error! : 알맞은 record가 아닙니다.\n");
		pass2_return = -1;
	    }
	}
    }
    fclose(fp);
    return pass2_return;
}


