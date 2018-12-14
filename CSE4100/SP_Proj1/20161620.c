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
    signed long temp;
    FILE *fp;
    unsigned int id;
    Node* pnode = NULL;//출력때 이용할 포인터
    Table* new = NULL;

    while(1)
    {
	for(i=0; i<MAX; i++)
	{
	    command[i] = '\0';
	    mnemonic[i] = '\0';
	}
	printf("\nsicsim> ");
	fgets(command,MAX,stdin);//공백까지 읽으려고 fgets 사용
	strcpy(store_command,command);
	store_command[strlen(store_command)-1]='\0';
	start = end = value = address = -1;
	///opcode 파일처리///
	fp = fopen("opcode.txt","r");
	while(1)
	{
	    if(fscanf(fp,"%X %s %s\n",&key,name,format)==EOF)
		break;
	    else
	    {
		new = Create_Table(key,name);
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
    char store[MAX] = {'\0',};
    char* ptr;
    
    command[(strlen(command)-1)] = '\0';//fgets로 받았으니까 우선 개행 문자 짜름

    cut_space(command);//앞뒤 공백 있는것 자름
    strcpy(store2,command);//앞뒤 없고 사이에는 공백 있는것

    for(i=0; (unsigned)i<strlen(command); i++)
    {
	if(start_flag == 0)
	{
	    if((command[i]>='A' && command[i]<='Z') ||(command[i]>='a' && command[i]<='z') || (command[i]>='0' && command[i]<='9') || command[i]==',' || command[i]=='.')
	    {
		if(command[i]==' ' || command[i]=='\t')
		    continue;
		if(command[i]>='A' && command[i]<='Z')
		    command[i] += 32;
		store[j] = command[i];
		j++;
		start_flag=1; //영어 대소문자, 숫자나 콤마 입력이 처음 들어옴
	    }
	    else
		return_flag1 = -1; //정상 입력이 아님
	}
	else if(start_flag == 1)//문자가 시작되면 알파벳 외에 다른 문자가 들어오면 적절하지 않은 명령어 처리;;-1 리턴
	{
	    if(command[i]==' ' || command[i]=='\t')
		continue;
	    if((command[i]>='A' && command[i]<='Z') || (command[i]>='a' && command[i]<='z') || (command[i]>='0' && command[i]<='9') || command[i]==',' || command[i]=='.')
	    {
		if(command[i]>='A' && command[i]<='Z')
		    command[i] += 32;
		store[j]=command[i];
		j++;
	    }
	    else
		return_flag1 = -1;
	}
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

	strcpy(dumpRange,store2+i+1);//dump 명령어 이후의 것만 dumpRange에 저장
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

Table* Create_Table(int key, char* name)
{
    Table* new = (Table*)malloc(sizeof(Table));
    new->id = key;
    strcpy(new->instruct,name);
    new->hashNext=NULL;

    return new;
}//테이블에 필요한 노드 생성

void insertBack(Node** head, Node** tail, Node* newnode)
{
    if((*head) == NULL)
    {
	(*head) = newnode;
	(*tail) = newnode;
    }
    else
    {
	(*tail)->link = newnode;
	(*tail) = newnode;
    }
}//히스토리에 이용하기 위한 연결리스트 삽입 함수

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
