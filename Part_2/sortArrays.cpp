#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <iostream>
#include <time.h>
#include <math.h>
#include <thread>
#include <mutex>
#include <condition_variable>

#include <queue> 

using namespace std;

struct page_table_entry{


	char isPageModified=0;
	char isPageReferenced=0;
	char isPagePresent=0;

	int pageNumberInRam=0;

	long long int instruction_counter_number=0;



};

queue<struct page_table_entry*> fifo_list;    //for list as fifo

int frame_size; // frame size

int pysicall_memory_frames_number;  //how much frames pysicall memory have

int virtual_memory_frames_number;   //how much frames virtual memory have

struct page_table_entry *page_table;//page table 

int *pysicall_memory;               //pysicall memory

char *page_replacement_algorithm_name;  //specify name of replacement algorithm

char *pageReplacement;              // specify policy of replacement algorithm

FILE *fp;							// File pointer will be point diskFİle


int statistics_for_threads[6][6];  // statistics for threads

int print_table_in_each;           // specify printing page table occurs on how much number

long long int memory_access_counter=0;       // count on each memory reference(for printing page table)
int memory_access_counter2=0;	   // count on each memory reference(for NRU)

long long int instruction_counter_total=0;   // this variable responsible for count the instructions


int total_number=0;

int total_number2=0;

std::mutex access_operation;

int page_replacement(int index_of_page_in_virtual_memory,char *tName,char *pageReplacementt){

	int tNameindex=0;  //we will change statistics seperately for every thread(just regular threads like quick, merge etc)

	if(strcmp(tName,"bubble")==0)
		tNameindex=1;
	else if(strcmp(tName,"quick")==0)
		tNameindex=2;

	else if(strcmp(tName,"merge")==0)
		tNameindex=3;

	else if(strcmp(tName,"index")==0)
		tNameindex=4;



	if(strcmp(page_replacement_algorithm_name,"FIFO")==0){

		statistics_for_threads[tNameindex][5]=statistics_for_threads[tNameindex][5]+1; //increment number of page replacements for related thread

		int size_of_linked_list=fifo_list.size();

		

		if(size_of_linked_list==pysicall_memory_frames_number){



		if(strcmp(pageReplacementt,"global")==0)	{

			int index_of_page_in_ram=fifo_list.front()->pageNumberInRam;

			int count=0;
			for(int i=0;i<virtual_memory_frames_number;++i){
				if(&page_table[i]==fifo_list.front())
					break;

				count+=1;
			}
		if(fifo_list.front()->isPageModified=='1'){

			statistics_for_threads[tNameindex][3]=statistics_for_threads[tNameindex][3]+1; //increment number of disk page writes
			fseek(fp,count*frame_size*4,SEEK_SET);


			fwrite(&pysicall_memory[index_of_page_in_ram*frame_size],sizeof(int),frame_size,fp);

		}	

			page_table[count].isPagePresent='0';		//yeni
			page_table[index_of_page_in_virtual_memory].isPagePresent='1';

			page_table[index_of_page_in_virtual_memory].isPageModified='0';
			page_table[index_of_page_in_virtual_memory].isPageReferenced='0';

			page_table[index_of_page_in_virtual_memory].pageNumberInRam=index_of_page_in_ram;

			fseek(fp,index_of_page_in_virtual_memory*frame_size*4,SEEK_SET);

			fread(&pysicall_memory[index_of_page_in_ram*frame_size],sizeof(int),frame_size,fp);	

			statistics_for_threads[tNameindex][2]=statistics_for_threads[tNameindex][2]+1; //increment number of disk page reads

			fifo_list.pop();		

			fifo_list.push(&page_table[index_of_page_in_virtual_memory]);


			return 5;

		}

		else {

			queue<struct page_table_entry*> temp_queue=fifo_list;

			int fifo_list_size=fifo_list.size();

			struct page_table_entry *temp3;

			if(strcmp(tName,"quick")==0){

				int i=0;
				while(i!=fifo_list_size){

					temp3=temp_queue.front();
					temp_queue.pop();
					for(int i=virtual_memory_frames_number/4;i<(virtual_memory_frames_number/4)*2;++i){

						if(temp3==&page_table[i]){

							int index_of_page_in_ram= temp3->pageNumberInRam;
							int count=0;
							for(int i=0;i<virtual_memory_frames_number;++i){
								if(&page_table[i]==temp3)
									break;

								count+=1;
							}

							if(temp3->isPageModified=='1'){

							fseek(fp,count*frame_size*4,SEEK_SET);
							fwrite(&pysicall_memory[index_of_page_in_ram*frame_size],sizeof(int),frame_size,fp);

							statistics_for_threads[tNameindex][3]=statistics_for_threads[tNameindex][3]+1;//increment number of disk page writes
							}

							page_table[count].isPagePresent='0';
							page_table[index_of_page_in_virtual_memory].isPagePresent='1';

							page_table[index_of_page_in_virtual_memory].isPageModified='0';
							page_table[index_of_page_in_virtual_memory].isPageReferenced='0';

							page_table[index_of_page_in_virtual_memory].pageNumberInRam=index_of_page_in_ram;

							fseek(fp,index_of_page_in_virtual_memory*frame_size*4,SEEK_SET);

							fread(&pysicall_memory[index_of_page_in_ram*frame_size],sizeof(int),frame_size,fp);							

							statistics_for_threads[tNameindex][2]=statistics_for_threads[tNameindex][2]+1;//increment number of disk page reads
							queue<struct page_table_entry*> temp_queue2;
							queue<struct page_table_entry*> temp_queue3=fifo_list;

							for(int i=0;i<fifo_list.size();++i){
								if(temp_queue3.front()!=temp3){

									temp_queue2.push(temp_queue3.front());
								}

								temp_queue3.pop();
							}
							queue<struct page_table_entry*> temp_queue4;
							for(int i=0;temp_queue2.size()!=0;++i){

								temp_queue4.push(temp_queue2.front());
								temp_queue2.pop();
							}

							temp_queue4.push(&page_table[index_of_page_in_virtual_memory]);

							fifo_list=temp_queue4;

							return 5;
						}
					}

					i++;

				}


			int index_of_page_in_ram=fifo_list.front()->pageNumberInRam;

			int count=0;
			for(int i=0;i<virtual_memory_frames_number;++i){
				if(&page_table[i]==fifo_list.front())
					break;

				count+=1;
			}

			if(fifo_list.front()->isPageModified=='1'){

				fseek(fp,count*frame_size*4,SEEK_SET);

				fwrite(&pysicall_memory[index_of_page_in_ram*frame_size],sizeof(int),frame_size,fp);

				statistics_for_threads[tNameindex][3]=statistics_for_threads[tNameindex][3]+1;//increment number of disk page writes
			}	

			page_table[count].isPagePresent='0';
			page_table[index_of_page_in_virtual_memory].isPagePresent='1';

			page_table[index_of_page_in_virtual_memory].isPageModified='0';
			page_table[index_of_page_in_virtual_memory].isPageReferenced='0';

			page_table[index_of_page_in_virtual_memory].pageNumberInRam=index_of_page_in_ram;

			fseek(fp,index_of_page_in_virtual_memory*frame_size*4,SEEK_SET);

			fread(&pysicall_memory[index_of_page_in_ram*frame_size],sizeof(int),frame_size,fp);		

			statistics_for_threads[tNameindex][2]=statistics_for_threads[tNameindex][2]+1;	//increment number of disk page reads

			fifo_list.pop();		

			fifo_list.push(&page_table[index_of_page_in_virtual_memory]);

			return 5;


			}


			else if(strcmp(tName,"bubble")==0){

				int i=0;
				while(i!=fifo_list_size){

					temp3=temp_queue.front();
					temp_queue.pop();
					for(int i=0;i<virtual_memory_frames_number/4;++i){

						if(temp3==&page_table[i]){

							int index_of_page_in_ram= temp3->pageNumberInRam;

							int count=0;
							for(int i=0;i<virtual_memory_frames_number;++i){
								if(&page_table[i]==temp3)
									break;

								count+=1;
							}

							if(temp3->isPageModified=='1'){

							fseek(fp,count*frame_size*4,SEEK_SET);
							fwrite(&pysicall_memory[index_of_page_in_ram*frame_size],sizeof(int),frame_size,fp);

							statistics_for_threads[tNameindex][3]=statistics_for_threads[tNameindex][3]+1;//increment number of disk page writes
							}

							page_table[count].isPagePresent='0';

							page_table[index_of_page_in_virtual_memory].isPagePresent='1';

							page_table[index_of_page_in_virtual_memory].isPageModified='0';
							page_table[index_of_page_in_virtual_memory].isPageReferenced='0';

							page_table[index_of_page_in_virtual_memory].pageNumberInRam=index_of_page_in_ram;

							fseek(fp,index_of_page_in_virtual_memory*frame_size*4,SEEK_SET);

							fread(&pysicall_memory[index_of_page_in_ram*frame_size],sizeof(int),frame_size,fp);

							statistics_for_threads[tNameindex][2]=statistics_for_threads[tNameindex][2]+1;//increment number of disk page reads


							queue<struct page_table_entry*> temp_queue2;
							queue<struct page_table_entry*> temp_queue3=fifo_list;

							for(int i=0;i<fifo_list.size();++i){
								if(temp_queue3.front()!=temp3){

									temp_queue2.push(temp_queue3.front());
								}

								temp_queue3.pop();
							}
							queue<struct page_table_entry*> temp_queue4;
							for(int i=0;temp_queue2.size()!=0;++i){

								temp_queue4.push(temp_queue2.front());
								temp_queue2.pop();
							}

							temp_queue4.push(&page_table[index_of_page_in_virtual_memory]);

							fifo_list=temp_queue4;

							return 5;
						}
					}

					i++;

				}


			int index_of_page_in_ram=fifo_list.front()->pageNumberInRam;

			int count=0;
			for(int i=0;i<virtual_memory_frames_number;++i){
				if(&page_table[i]==fifo_list.front())
					break;

				count+=1;
			}

			if(fifo_list.front()->isPageModified=='1'){

				fseek(fp,count*frame_size*4,SEEK_SET);

				fwrite(&pysicall_memory[index_of_page_in_ram*frame_size],sizeof(int),frame_size,fp);

				statistics_for_threads[tNameindex][3]=statistics_for_threads[tNameindex][3]+1;//increment number of disk page writes
			}	

			page_table[count].isPagePresent='0';
			page_table[index_of_page_in_virtual_memory].isPagePresent='1';

			page_table[index_of_page_in_virtual_memory].isPageModified='0';
			page_table[index_of_page_in_virtual_memory].isPageReferenced='0';

			page_table[index_of_page_in_virtual_memory].pageNumberInRam=index_of_page_in_ram;

			fseek(fp,index_of_page_in_virtual_memory*frame_size*4,SEEK_SET);

			fread(&pysicall_memory[index_of_page_in_ram*frame_size],sizeof(int),frame_size,fp);	

			statistics_for_threads[tNameindex][2]=statistics_for_threads[tNameindex][2]+1;	//increment number of disk page reads	

			fifo_list.pop();		

			fifo_list.push(&page_table[index_of_page_in_virtual_memory]);

			return 5;

			}


			else if(strcmp(tName,"merge")==0){

				int i=0;
				while(i!=fifo_list_size){

					temp3=temp_queue.front();
					temp_queue.pop();
					//looking for a page that has data for related thread
					for(int i=2*(virtual_memory_frames_number/4);i<(virtual_memory_frames_number/4)*3;++i){

						if(temp3==&page_table[i]){

							int index_of_page_in_ram= temp3->pageNumberInRam;

							int count=0;
							for(int i=0;i<virtual_memory_frames_number;++i){
								if(&page_table[i]==temp3)
									break;

								count+=1;
							}

							if(temp3->isPageModified=='1'){

							fseek(fp,count*frame_size*4,SEEK_SET);
							fwrite(&pysicall_memory[index_of_page_in_ram*frame_size],sizeof(int),frame_size,fp);

							statistics_for_threads[tNameindex][3]=statistics_for_threads[tNameindex][3]+1;//increment number of disk page writes
							}
							page_table[count].isPagePresent='0';
							page_table[index_of_page_in_virtual_memory].isPagePresent='1';

							page_table[index_of_page_in_virtual_memory].isPageModified='0';
							page_table[index_of_page_in_virtual_memory].isPageReferenced='0';

							page_table[index_of_page_in_virtual_memory].pageNumberInRam=index_of_page_in_ram;

							fseek(fp,index_of_page_in_virtual_memory*frame_size*4,SEEK_SET);

							fread(&pysicall_memory[index_of_page_in_ram*frame_size],sizeof(int),frame_size,fp);	

							statistics_for_threads[tNameindex][2]=statistics_for_threads[tNameindex][2]+1;//increment number of disk page reads						

							queue<struct page_table_entry*> temp_queue2;
							queue<struct page_table_entry*> temp_queue3=fifo_list;

							for(int i=0;i<fifo_list.size();++i){
								if(temp_queue3.front()!=temp3){

									temp_queue2.push(temp_queue3.front());
								}

								temp_queue3.pop();
							}
							queue<struct page_table_entry*> temp_queue4;
							for(int i=0;temp_queue2.size()!=0;++i){

								temp_queue4.push(temp_queue2.front());
								temp_queue2.pop();
							}

							temp_queue4.push(&page_table[index_of_page_in_virtual_memory]);

							fifo_list=temp_queue4;

							return 5;
						}
					}

					i++;

				}

			//if we cant not found a page that has data for related thread then  we apply page replacement like global
			int index_of_page_in_ram=fifo_list.front()->pageNumberInRam;
			int count=0;
			for(int i=0;i<virtual_memory_frames_number;++i){
				if(&page_table[i]==fifo_list.front())
					break;

				count+=1;
			}

			if(fifo_list.front()->isPageModified=='1'){

				fseek(fp,count*frame_size*4,SEEK_SET);

				fwrite(&pysicall_memory[index_of_page_in_ram*frame_size],sizeof(int),frame_size,fp);

				statistics_for_threads[tNameindex][3]=statistics_for_threads[tNameindex][3]+1;//increment number of disk page writes
			}	

			page_table[count].isPagePresent='0';
			page_table[index_of_page_in_virtual_memory].isPagePresent='1';

			page_table[index_of_page_in_virtual_memory].isPageModified='0';
			page_table[index_of_page_in_virtual_memory].isPageReferenced='0';

			page_table[index_of_page_in_virtual_memory].pageNumberInRam=index_of_page_in_ram;

			fseek(fp,index_of_page_in_virtual_memory*frame_size*4,SEEK_SET);

			fread(&pysicall_memory[index_of_page_in_ram*frame_size],sizeof(int),frame_size,fp);	

			statistics_for_threads[tNameindex][2]=statistics_for_threads[tNameindex][2]+1;//increment number of disk page reads	

			fifo_list.pop();		

			fifo_list.push(&page_table[index_of_page_in_virtual_memory]);

			return 5;

			}


			else if(strcmp(tName,"index")==0){

			int i=0;
				while(i!=fifo_list_size){

					temp3=temp_queue.front();
					temp_queue.pop();

					//looking for a page that has data for related thread
					for(int i=3*(virtual_memory_frames_number/4);i<(virtual_memory_frames_number/4)*4;++i){

						if(temp3==&page_table[i]){

							int index_of_page_in_ram= temp3->pageNumberInRam;

							int count=0;
							for(int i=0;i<virtual_memory_frames_number;++i){
								if(&page_table[i]==temp3)
									break;

								count+=1;
							}

							if(temp3->isPageModified=='1'){

							fseek(fp,count*frame_size*4,SEEK_SET);
							fwrite(&pysicall_memory[index_of_page_in_ram*frame_size],sizeof(int),frame_size,fp);
							statistics_for_threads[tNameindex][3]=statistics_for_threads[tNameindex][3]+1;//increment number of disk page writes
							}

							page_table[count].isPagePresent='0';
							page_table[index_of_page_in_virtual_memory].isPagePresent='1';

							page_table[index_of_page_in_virtual_memory].isPageModified='0';
							page_table[index_of_page_in_virtual_memory].isPageReferenced='0';

							page_table[index_of_page_in_virtual_memory].pageNumberInRam=index_of_page_in_ram;

							fseek(fp,index_of_page_in_virtual_memory*frame_size*4,SEEK_SET);

							fread(&pysicall_memory[index_of_page_in_ram*frame_size],sizeof(int),frame_size,fp);

							statistics_for_threads[tNameindex][2]=statistics_for_threads[tNameindex][2]+1;//increment number of disk page reads

							queue<struct page_table_entry*> temp_queue2;
							queue<struct page_table_entry*> temp_queue3=fifo_list;

							for(int i=0;i<fifo_list.size();++i){
								if(temp_queue3.front()!=temp3){

									temp_queue2.push(temp_queue3.front());
								}

								temp_queue3.pop();
							}
							queue<struct page_table_entry*> temp_queue4;
							for(int i=0;temp_queue2.size()!=0;++i){

								temp_queue4.push(temp_queue2.front());
								temp_queue2.pop();
							}

							temp_queue4.push(&page_table[index_of_page_in_virtual_memory]);

							fifo_list=temp_queue4;

							return 5;
						}
					}

					i++;

				}

			//if we cant not found a page that has data for related thread then  we apply page replacement like global
			int index_of_page_in_ram=fifo_list.front()->pageNumberInRam;

			int count=0;
			for(int i=0;i<virtual_memory_frames_number;++i){
				if(&page_table[i]==fifo_list.front())
					break;

				count+=1;
			}

			if(fifo_list.front()->isPageModified=='1'){

				fseek(fp,count*frame_size*4,SEEK_SET);

				fwrite(&pysicall_memory[index_of_page_in_ram*frame_size],sizeof(int),frame_size,fp);

				statistics_for_threads[tNameindex][3]=statistics_for_threads[tNameindex][3]+1;//increment number of disk page writes
			}	

			page_table[count].isPagePresent='0';

			page_table[index_of_page_in_virtual_memory].isPagePresent='1';

			page_table[index_of_page_in_virtual_memory].isPageModified='0';
			page_table[index_of_page_in_virtual_memory].isPageReferenced='0';

			page_table[index_of_page_in_virtual_memory].pageNumberInRam=index_of_page_in_ram;

			fseek(fp,index_of_page_in_virtual_memory*frame_size*4,SEEK_SET);

			fread(&pysicall_memory[index_of_page_in_ram*frame_size],sizeof(int),frame_size,fp);

			statistics_for_threads[tNameindex][2]=statistics_for_threads[tNameindex][2]+1;//increment number of disk page reads


			fifo_list.pop();		

			fifo_list.push(&page_table[index_of_page_in_virtual_memory]);


			}

		}

		}
		else{
			//we increment this counter at the top. But if memory is not full yet then this means there is no page replacement("replacement")
			statistics_for_threads[tNameindex][5]=statistics_for_threads[tNameindex][5]-1;
			
			int counter=0;
			for(int i=0;i<virtual_memory_frames_number;++i){

				if(page_table[i].isPagePresent=='1')
					counter+=1;
			}

			fseek(fp,frame_size*index_of_page_in_virtual_memory*4,SEEK_SET);

			fread(&pysicall_memory[counter*frame_size],sizeof(int),frame_size,fp);

			statistics_for_threads[tNameindex][2]=statistics_for_threads[tNameindex][2]+1;//increment number of disk page reads

			page_table[index_of_page_in_virtual_memory].isPagePresent='1';
			page_table[index_of_page_in_virtual_memory].pageNumberInRam=counter;

			fifo_list.push(&page_table[index_of_page_in_virtual_memory]);

			return 5;

		}


	}


	else if(strcmp(page_replacement_algorithm_name,"SC")==0){

		statistics_for_threads[tNameindex][5]=statistics_for_threads[tNameindex][5]+1;

		int size_of_linked_list=fifo_list.size();
		

		if(size_of_linked_list==pysicall_memory_frames_number){

		if(strcmp(pageReplacementt,"global")==0)	{


		if(fifo_list.front()->isPageReferenced=='1'){

			
			fifo_list.front()->isPageReferenced='0';  // set R value to 0

			struct page_table_entry *temp=fifo_list.front();
			fifo_list.pop();

			fifo_list.push(temp);

			return page_replacement(index_of_page_in_virtual_memory,tName,pageReplacementt);

		}	


		int index_of_page_in_ram=fifo_list.front()->pageNumberInRam;

		int count=0;
		for(int i=0;i<virtual_memory_frames_number;++i){
			if(&page_table[i]==fifo_list.front())
				break;

			count+=1;
		}


		if(fifo_list.front()->isPageModified=='1'){

			fseek(fp,count*frame_size*4,SEEK_SET);
			fwrite(&pysicall_memory[index_of_page_in_ram*frame_size],sizeof(int),frame_size,fp);

			statistics_for_threads[tNameindex][3]=statistics_for_threads[tNameindex][3]+1;//increment number of disk page writes
		}

			page_table[count].isPagePresent='0';

			page_table[index_of_page_in_virtual_memory].isPagePresent='1';

			page_table[index_of_page_in_virtual_memory].isPageModified='0';
			page_table[index_of_page_in_virtual_memory].isPageReferenced='0';

			page_table[index_of_page_in_virtual_memory].pageNumberInRam=index_of_page_in_ram;

			fseek(fp,index_of_page_in_virtual_memory*frame_size*4,SEEK_SET);

			fread(&pysicall_memory[index_of_page_in_ram*frame_size],sizeof(int),frame_size,fp);

			statistics_for_threads[tNameindex][2]=statistics_for_threads[tNameindex][2]+1;//increment number of disk page reads

			fifo_list.pop();
			fifo_list.push(&page_table[index_of_page_in_virtual_memory]);		

		}

		else {

				queue<struct page_table_entry*> temp_queue=fifo_list;

				int fifo_list_size=fifo_list.size();
				struct page_table_entry *temp3;

				int i=0;
				while(i!=fifo_list_size){

					temp3=temp_queue.front();
					temp_queue.pop();

					int start=0;
					int end=0;


					if(strcmp(tName,"bubble")==0){

						start=0;
						end=virtual_memory_frames_number/4;
					}
					else if(strcmp(tName,"quick")==0){
						start=virtual_memory_frames_number/4;
						end=(virtual_memory_frames_number/4)*2;
					}
					else if(strcmp(tName,"merge")==0){
						start=(virtual_memory_frames_number/4)*2;
						end=(virtual_memory_frames_number/4)*3;

					}
					else if(strcmp(tName,"index")==0){
						start=(virtual_memory_frames_number/4)*3;
						end=virtual_memory_frames_number;

					}
					//looking for a page that has data for related thread
					for(int i=start;i<end;++i){

						if(temp3==&page_table[i]){

							if(temp3->isPageReferenced=='1'){

		
							temp3->isPageReferenced='0';  // set R value to 0

							queue<struct page_table_entry*> temp_queue2;
							queue<struct page_table_entry*> temp_queue3=fifo_list;

							for(int i=0;i<fifo_list.size();++i){
								if(temp_queue3.front()!=temp3){

									temp_queue2.push(temp_queue3.front());
								}

								temp_queue3.pop();
							}
							queue<struct page_table_entry*> temp_queue4;
							for(int i=0;temp_queue2.size()!=0;++i){

								temp_queue4.push(temp_queue2.front());
								temp_queue2.pop();
							}

							struct page_table_entry *temp10=temp3;
							temp_queue4.push(temp10);

							fifo_list=temp_queue4;


							return page_replacement(index_of_page_in_virtual_memory,tName,pageReplacementt);

							}	

							int index_of_page_in_ram= temp3->pageNumberInRam;

							int count=0;
							for(int i=0;i<virtual_memory_frames_number;++i){
								if(&page_table[i]==temp3)
									break;

								count+=1;
							}

							if(temp3->isPageModified=='1'){

							fseek(fp,count*frame_size*4,SEEK_SET);
							fwrite(&pysicall_memory[index_of_page_in_ram*frame_size],sizeof(int),frame_size,fp);

							statistics_for_threads[tNameindex][3]=statistics_for_threads[tNameindex][3]+1;//increment number of disk page writes
							}

							page_table[count].isPagePresent='0';

							page_table[index_of_page_in_virtual_memory].isPagePresent='1';

							page_table[index_of_page_in_virtual_memory].isPageModified='0';
							page_table[index_of_page_in_virtual_memory].isPageReferenced='0';

							page_table[index_of_page_in_virtual_memory].pageNumberInRam=index_of_page_in_ram;

							fseek(fp,index_of_page_in_virtual_memory*frame_size*4,SEEK_SET);

							fread(&pysicall_memory[index_of_page_in_ram*frame_size],sizeof(int),frame_size,fp);

							statistics_for_threads[tNameindex][2]=statistics_for_threads[tNameindex][2]+1;//increment number of disk page reads

							queue<struct page_table_entry*> temp_queue2;
							queue<struct page_table_entry*> temp_queue3=fifo_list;

							for(int i=0;i<fifo_list.size();++i){
								if(temp_queue3.front()!=temp3){

									temp_queue2.push(temp_queue3.front());
								}

								temp_queue3.pop();
							}
							queue<struct page_table_entry*> temp_queue4;
							for(int i=0;temp_queue2.size()!=0;++i){

								temp_queue4.push(temp_queue2.front());
								temp_queue2.pop();
							}

							temp_queue4.push(&page_table[index_of_page_in_virtual_memory]);

							fifo_list=temp_queue4;

							return 5;
						}
					}

					i++;

				}
			//if we cant not found a page that has data for related thread then  we apply page replacement like global
			if(fifo_list.front()->isPageReferenced=='1'){

			

				fifo_list.front()->isPageReferenced='0';  // set R value to 0

				struct page_table_entry *temp=fifo_list.front();
				fifo_list.pop();

				fifo_list.push(temp);

				return page_replacement(index_of_page_in_virtual_memory,tName,pageReplacementt);

			}	

			int index_of_page_in_ram=fifo_list.front()->pageNumberInRam;

			int count=0;
			for(int i=0;i<virtual_memory_frames_number;++i){
				if(&page_table[i]==fifo_list.front())
					break;

				count+=1;
			}

			if(fifo_list.front()->isPageModified=='1'){

				fseek(fp,count*frame_size*4,SEEK_SET);

				fwrite(&pysicall_memory[index_of_page_in_ram*frame_size],sizeof(int),frame_size,fp);

				statistics_for_threads[tNameindex][3]=statistics_for_threads[tNameindex][3]+1;//increment number of disk page writes
			}	

			page_table[count].isPagePresent='0';

			page_table[index_of_page_in_virtual_memory].isPagePresent='1';

			page_table[index_of_page_in_virtual_memory].isPageModified='0';
			page_table[index_of_page_in_virtual_memory].isPageReferenced='0';

			page_table[index_of_page_in_virtual_memory].pageNumberInRam=index_of_page_in_ram;

			fseek(fp,index_of_page_in_virtual_memory*frame_size*4,SEEK_SET);

			fread(&pysicall_memory[index_of_page_in_ram*frame_size],sizeof(int),frame_size,fp);

			statistics_for_threads[tNameindex][2]=statistics_for_threads[tNameindex][2]+1;//increment number of disk page reads


			fifo_list.pop();		

			fifo_list.push(&page_table[index_of_page_in_virtual_memory]);


		}


		}
		else{

			statistics_for_threads[tNameindex][5]=statistics_for_threads[tNameindex][5]-1; //we increment this counter at the top. But if memory is not full yet then this means there is no page replacement("replacement")
			int counter=0;
			for(int i=0;i<virtual_memory_frames_number;++i){

				if(page_table[i].isPagePresent=='1')
					counter+=1;
			}

			fseek(fp,frame_size*index_of_page_in_virtual_memory*4,SEEK_SET);

			fread(&pysicall_memory[counter*frame_size],sizeof(int),frame_size,fp);

			statistics_for_threads[tNameindex][2]=statistics_for_threads[tNameindex][2]+1;//increment number of disk page reads

			page_table[index_of_page_in_virtual_memory].isPagePresent='1';

			page_table[index_of_page_in_virtual_memory].pageNumberInRam=counter;

			fifo_list.push(&page_table[index_of_page_in_virtual_memory]);

		}

	}


	else if(strcmp(page_replacement_algorithm_name,"LRU")==0){


		statistics_for_threads[tNameindex][5]=statistics_for_threads[tNameindex][5]+1;

		//determine memory is full or not
		int filled_memory_page_numbers=0;

		for(int i=0;i<virtual_memory_frames_number;++i){

			if(page_table[i].isPagePresent=='1')
				filled_memory_page_numbers+=1;

		}


		//if pyscicall memory is full
		if(filled_memory_page_numbers==pysicall_memory_frames_number){


		if(strcmp(pageReplacementt,"global")==0){	


		int a=0;
		int lowest_instruction_counter_number;
		int page_index_with_lowest_instruction_counter;

		for(;a<virtual_memory_frames_number;++a){

			if(page_table[a].isPagePresent=='1'){

				lowest_instruction_counter_number=page_table[a].instruction_counter_number;
				page_index_with_lowest_instruction_counter=a;
				break;
			}
		}
		//look for page that has lowest instruction counter
		for(int i=a;i<virtual_memory_frames_number;++i){

			if(page_table[i].isPagePresent=='1' && (page_table[i].instruction_counter_number<lowest_instruction_counter_number)){
				lowest_instruction_counter_number=page_table[i].instruction_counter_number;
				page_index_with_lowest_instruction_counter=i;
			}

		}

		int index_of_page_in_ram=page_table[page_index_with_lowest_instruction_counter].pageNumberInRam;

		if(page_table[page_index_with_lowest_instruction_counter].isPagePresent=='1' && page_table[page_index_with_lowest_instruction_counter].isPageModified=='1'){
			statistics_for_threads[tNameindex][3]=statistics_for_threads[tNameindex][3]+1;//increment number of disk page writes
			fseek(fp,page_index_with_lowest_instruction_counter*frame_size*4,SEEK_SET);
			fwrite(&pysicall_memory[index_of_page_in_ram*frame_size],sizeof(int),frame_size,fp);

		}

		page_table[page_index_with_lowest_instruction_counter].isPagePresent='0';

		page_table[index_of_page_in_virtual_memory].isPagePresent='1';
		page_table[index_of_page_in_virtual_memory].isPageModified='0';
		page_table[index_of_page_in_virtual_memory].isPageReferenced='0';


		page_table[index_of_page_in_virtual_memory].pageNumberInRam=index_of_page_in_ram;

		fseek(fp,index_of_page_in_virtual_memory*frame_size*4,SEEK_SET);

		fread(&pysicall_memory[index_of_page_in_ram*frame_size],sizeof(int),frame_size,fp);
		statistics_for_threads[tNameindex][2]=statistics_for_threads[tNameindex][2]+1; //increment number of disk page reads
		return 6;


		}


		else{

		int start=0;
		int end=0;

		if(strcmp(tName,"bubble")==0){

			start=0;
			end=virtual_memory_frames_number/4;
		}
		else if(strcmp(tName,"quick")==0){
			start=virtual_memory_frames_number/4;
			end=(virtual_memory_frames_number/4)*2;
		}
		else if(strcmp(tName,"merge")==0){
			start=(virtual_memory_frames_number/4)*2;
			end=(virtual_memory_frames_number/4)*3;

		}
		else if(strcmp(tName,"index")==0){
			start=(virtual_memory_frames_number/4)*3;
			end=virtual_memory_frames_number;

		}

		int a=start;
		int lowest_instruction_counter_number;
		int page_index_with_lowest_instruction_counter=-2;

		for(;a<end;++a){

			if(page_table[a].isPagePresent=='1'){

				lowest_instruction_counter_number=page_table[a].instruction_counter_number;
				page_index_with_lowest_instruction_counter=a;
				break;
			}
		}

		for(int i=a;i<end;++i){

			if(page_table[i].isPagePresent=='1' && (page_table[i].instruction_counter_number<lowest_instruction_counter_number)){
				lowest_instruction_counter_number=page_table[i].instruction_counter_number;
				page_index_with_lowest_instruction_counter=i;
			}

		}

		//if we cant not found a page that has data for related thread then  we apply page replacement like global
		if(page_index_with_lowest_instruction_counter==-2){


		a=0;
		lowest_instruction_counter_number;
		page_index_with_lowest_instruction_counter=-2;

		for(;a<virtual_memory_frames_number;++a){

			if(page_table[a].isPagePresent=='1'){

				lowest_instruction_counter_number=page_table[a].instruction_counter_number;
				page_index_with_lowest_instruction_counter=a;
				break;
			}
		}
		//look for page that has lowest instruction counter
		for(int i=a;i<virtual_memory_frames_number;++i){

			if(page_table[i].isPagePresent=='1' && (page_table[i].instruction_counter_number<lowest_instruction_counter_number)){
				lowest_instruction_counter_number=page_table[i].instruction_counter_number;
				page_index_with_lowest_instruction_counter=i;
			}

		}

		}

		int index_of_page_in_ram=page_table[page_index_with_lowest_instruction_counter].pageNumberInRam;

		if(page_table[page_index_with_lowest_instruction_counter].isPagePresent=='1' && page_table[page_index_with_lowest_instruction_counter].isPageModified=='1'){
			statistics_for_threads[tNameindex][3]=statistics_for_threads[tNameindex][3]+1; //increment number of disk page writes
			fseek(fp,page_index_with_lowest_instruction_counter*frame_size*4,SEEK_SET);
			fwrite(&pysicall_memory[index_of_page_in_ram*frame_size],sizeof(int),frame_size,fp);

		}

		page_table[page_index_with_lowest_instruction_counter].isPagePresent='0';

		page_table[index_of_page_in_virtual_memory].isPagePresent='1';
		page_table[index_of_page_in_virtual_memory].isPageModified='0';
		page_table[index_of_page_in_virtual_memory].isPageReferenced='0';


		page_table[index_of_page_in_virtual_memory].pageNumberInRam=index_of_page_in_ram;

		fseek(fp,index_of_page_in_virtual_memory*frame_size*4,SEEK_SET);

		fread(&pysicall_memory[index_of_page_in_ram*frame_size],sizeof(int),frame_size,fp);
		statistics_for_threads[tNameindex][2]=statistics_for_threads[tNameindex][2]+1; //increment number of disk page reads
		return 6;

		}


		}


		else{

			statistics_for_threads[tNameindex][5]=statistics_for_threads[tNameindex][5]-1; //we increment this counter at the top. But if memory is not full yet then this means there is no page replacement("replacement")
			int filled_memory_page_numbers=0;

			for(int i=0;i<virtual_memory_frames_number;++i){

				if(page_table[i].isPagePresent=='1')
					filled_memory_page_numbers+=1;

			}

			fseek(fp,frame_size*index_of_page_in_virtual_memory*4,SEEK_SET);

			fread(&pysicall_memory[filled_memory_page_numbers*frame_size],sizeof(int),frame_size,fp);

			statistics_for_threads[tNameindex][2]=statistics_for_threads[tNameindex][2]+1; //increment number of disk page reads

			page_table[index_of_page_in_virtual_memory].isPagePresent='1';

			page_table[index_of_page_in_virtual_memory].pageNumberInRam=filled_memory_page_numbers;

			return 4;

		}


	}


	else if(strcmp(page_replacement_algorithm_name,"NRU")==0){


		statistics_for_threads[tNameindex][5]=statistics_for_threads[tNameindex][5]+1;

		//determine memory is full or not
		int filled_memory_page_numbers=0;

		for(int i=0;i<virtual_memory_frames_number;++i){

			if(page_table[i].isPagePresent=='1')
				filled_memory_page_numbers+=1;

		}

		if(filled_memory_page_numbers==pysicall_memory_frames_number){

		if(strcmp(pageReplacementt,"global")==0){

			//looking for a page that has class0 properties and it is in the pysicall memory(Present)
			for(int i=0;i<virtual_memory_frames_number;++i){

				if(page_table[i].isPagePresent=='1' && page_table[i].isPageModified=='0' && page_table[i].isPageReferenced=='0'){

					page_table[i].isPagePresent='0';

					int index_of_page_in_ram=page_table[i].pageNumberInRam ;

					if(page_table[i].isPageModified=='1'){

						statistics_for_threads[tNameindex][3]=statistics_for_threads[tNameindex][3]+1; //increment number of disk page writes
						fseek(fp,i*frame_size*4,SEEK_SET);

						fwrite(&pysicall_memory[index_of_page_in_ram*frame_size],sizeof(int),frame_size,fp);
					}	


					page_table[index_of_page_in_virtual_memory].isPagePresent='1';

					page_table[index_of_page_in_virtual_memory].isPageModified='0';
					page_table[index_of_page_in_virtual_memory].isPageReferenced='0';

					page_table[index_of_page_in_virtual_memory].pageNumberInRam=index_of_page_in_ram;

					fseek(fp,index_of_page_in_virtual_memory*frame_size*4,SEEK_SET);

					fread(&pysicall_memory[index_of_page_in_ram*frame_size],sizeof(int),frame_size,fp);

					statistics_for_threads[tNameindex][2]=statistics_for_threads[tNameindex][2]+1; //increment number of disk page reads
					return 5;

				}

			}

			//looking for a page that has class1 properties and it is in the pysicall memory(Present)
			for(int i=0;i<virtual_memory_frames_number;++i){

				if(page_table[i].isPagePresent=='1' && page_table[i].isPageModified=='1' && page_table[i].isPageReferenced=='0'){
					page_table[i].isPagePresent='0';

					int index_of_page_in_ram=page_table[i].pageNumberInRam ;

					if(page_table[i].isPageModified=='1'){

						statistics_for_threads[tNameindex][3]=statistics_for_threads[tNameindex][3]+1; //increment number of disk page writes
						fseek(fp,i*frame_size*4,SEEK_SET);

						fwrite(&pysicall_memory[index_of_page_in_ram*frame_size],sizeof(int),frame_size,fp);
					}	


					page_table[index_of_page_in_virtual_memory].isPagePresent='1';

					page_table[index_of_page_in_virtual_memory].isPageModified='0';
					page_table[index_of_page_in_virtual_memory].isPageReferenced='0';

					page_table[index_of_page_in_virtual_memory].pageNumberInRam=index_of_page_in_ram;

					fseek(fp,index_of_page_in_virtual_memory*frame_size*4,SEEK_SET);

					fread(&pysicall_memory[index_of_page_in_ram*frame_size],sizeof(int),frame_size,fp);

					statistics_for_threads[tNameindex][2]=statistics_for_threads[tNameindex][2]+1; //increment number of disk page reads
					return 5;

				}

			}
			//looking for a page that has class2 properties and it is in the pysicall memory(Present)
			for(int i=0;i<virtual_memory_frames_number;++i){

				if(page_table[i].isPagePresent=='1' && page_table[i].isPageModified=='0' && page_table[i].isPageReferenced=='1'){

					page_table[i].isPagePresent='0';

					int index_of_page_in_ram=page_table[i].pageNumberInRam ;

					if(page_table[i].isPageModified=='1'){

						statistics_for_threads[tNameindex][3]=statistics_for_threads[tNameindex][3]+1; //increment number of disk page writes
						fseek(fp,i*frame_size*4,SEEK_SET);

						fwrite(&pysicall_memory[index_of_page_in_ram*frame_size],sizeof(int),frame_size,fp);
					}	


					page_table[index_of_page_in_virtual_memory].isPagePresent='1';

					page_table[index_of_page_in_virtual_memory].isPageModified='0';
					page_table[index_of_page_in_virtual_memory].isPageReferenced='0';

					page_table[index_of_page_in_virtual_memory].pageNumberInRam=index_of_page_in_ram;

					fseek(fp,index_of_page_in_virtual_memory*frame_size*4,SEEK_SET);

					fread(&pysicall_memory[index_of_page_in_ram*frame_size],sizeof(int),frame_size,fp);
					statistics_for_threads[tNameindex][2]=statistics_for_threads[tNameindex][2]+1; //increment number of disk page reads
					return 5;

				}

			}
			//looking for a page that has class3 properties and it is in the pysicall memory(Present)
			for(int i=0;i<virtual_memory_frames_number;++i){

				if(page_table[i].isPagePresent=='1' && page_table[i].isPageModified=='1' && page_table[i].isPageReferenced=='1'){

					int index_of_page_in_ram=page_table[i].pageNumberInRam ;

					if(page_table[i].isPageModified=='1'){

						statistics_for_threads[tNameindex][3]=statistics_for_threads[tNameindex][3]+1; //increment number of disk page writes
						fseek(fp,i*frame_size*4,SEEK_SET);

						fwrite(&pysicall_memory[index_of_page_in_ram*frame_size],sizeof(int),frame_size,fp);
					}	

					page_table[i].isPagePresent='0';
					page_table[index_of_page_in_virtual_memory].isPagePresent='1';

					page_table[index_of_page_in_virtual_memory].isPageModified='0';
					page_table[index_of_page_in_virtual_memory].isPageReferenced='0';

					page_table[index_of_page_in_virtual_memory].pageNumberInRam=index_of_page_in_ram;

					fseek(fp,index_of_page_in_virtual_memory*frame_size*4,SEEK_SET);

					fread(&pysicall_memory[index_of_page_in_ram*frame_size],sizeof(int),frame_size,fp);
					statistics_for_threads[tNameindex][2]=statistics_for_threads[tNameindex][2]+1; //increment number of disk page reads
					return 5;

				}

			}

		}

		else{

			int start=0;
			int end=0;
			if(strcmp(tName,"bubble")==0){

				start=0;
			end=virtual_memory_frames_number/4;
			}
			else if(strcmp(tName,"quick")==0){
				start=virtual_memory_frames_number/4;
				end=(virtual_memory_frames_number/4)*2;
			}
			else if(strcmp(tName,"merge")==0){
				start=(virtual_memory_frames_number/4)*2;
				end=(virtual_memory_frames_number/4)*3;

			}
			else if(strcmp(tName,"index")==0){
				start=(virtual_memory_frames_number/4)*3;
				end=virtual_memory_frames_number;

			}

			//looking for a page that has data for related thread

			//looking for a page that has class0 properties and it is in the pysicall memory(Present)
			for(int i=start;i<end;++i){

				if(page_table[i].isPagePresent=='1' && page_table[i].isPageModified=='0' && page_table[i].isPageReferenced=='0'){
					page_table[i].isPagePresent='0';

					int index_of_page_in_ram=page_table[i].pageNumberInRam ;

					if(page_table[i].isPageModified=='1'){

						statistics_for_threads[tNameindex][3]=statistics_for_threads[tNameindex][3]+1; //increment number of disk page writes
						fseek(fp,i*frame_size*4,SEEK_SET);

						fwrite(&pysicall_memory[index_of_page_in_ram*frame_size],sizeof(int),frame_size,fp);
					}	


					page_table[index_of_page_in_virtual_memory].isPagePresent='1';

					page_table[index_of_page_in_virtual_memory].isPageModified='0';
					page_table[index_of_page_in_virtual_memory].isPageReferenced='0';

					page_table[index_of_page_in_virtual_memory].pageNumberInRam=index_of_page_in_ram;

					fseek(fp,index_of_page_in_virtual_memory*frame_size*4,SEEK_SET);

					fread(&pysicall_memory[index_of_page_in_ram*frame_size],sizeof(int),frame_size,fp);

					statistics_for_threads[tNameindex][2]=statistics_for_threads[tNameindex][2]+1; //increment number of disk page reads
					return 5;

				}

			}

			//looking for a page that has class1 properties and it is in the pysicall memory(Present)
			for(int i=start;i<end;++i){

				if(page_table[i].isPagePresent=='1' && page_table[i].isPageModified=='1' && page_table[i].isPageReferenced=='0'){
					page_table[i].isPagePresent='0';

					int index_of_page_in_ram=page_table[i].pageNumberInRam ;

					if(page_table[i].isPageModified=='1'){

						statistics_for_threads[tNameindex][3]=statistics_for_threads[tNameindex][3]+1;//increment number of disk page writes
						fseek(fp,i*frame_size*4,SEEK_SET);

						fwrite(&pysicall_memory[index_of_page_in_ram*frame_size],sizeof(int),frame_size,fp);
					}	


					page_table[index_of_page_in_virtual_memory].isPagePresent='1';

					page_table[index_of_page_in_virtual_memory].isPageModified='0';
					page_table[index_of_page_in_virtual_memory].isPageReferenced='0';

					page_table[index_of_page_in_virtual_memory].pageNumberInRam=index_of_page_in_ram;

					fseek(fp,index_of_page_in_virtual_memory*frame_size*4,SEEK_SET);

					fread(&pysicall_memory[index_of_page_in_ram*frame_size],sizeof(int),frame_size,fp);
					statistics_for_threads[tNameindex][2]=statistics_for_threads[tNameindex][2]+1; //increment number of disk page reads
					return 5;

				}

			}
			//looking for a page that has class2 properties and it is in the pysicall memory(Present)
			for(int i=start;i<end;++i){

				if(page_table[i].isPagePresent=='1' && page_table[i].isPageModified=='0' && page_table[i].isPageReferenced=='1'){
					page_table[i].isPagePresent='0';

					int index_of_page_in_ram=page_table[i].pageNumberInRam ;

					if(page_table[i].isPageModified=='1'){
						statistics_for_threads[tNameindex][3]=statistics_for_threads[tNameindex][3]+1; //increment number of disk page writes
						fseek(fp,i*frame_size*4,SEEK_SET);

						fwrite(&pysicall_memory[index_of_page_in_ram*frame_size],sizeof(int),frame_size,fp);
					}	


					page_table[index_of_page_in_virtual_memory].isPagePresent='1';

					page_table[index_of_page_in_virtual_memory].isPageModified='0';
					page_table[index_of_page_in_virtual_memory].isPageReferenced='0';

					page_table[index_of_page_in_virtual_memory].pageNumberInRam=index_of_page_in_ram;

					fseek(fp,index_of_page_in_virtual_memory*frame_size*4,SEEK_SET);

					fread(&pysicall_memory[index_of_page_in_ram*frame_size],sizeof(int),frame_size,fp);
					statistics_for_threads[tNameindex][2]=statistics_for_threads[tNameindex][2]+1; //increment number of disk page reads
					return 5;

				}

			}
			//looking for a page that has class3 properties and it is in the pysicall memory(Present)
			for(int i=start;i<end;++i){

				if(page_table[i].isPagePresent=='1' && page_table[i].isPageModified=='1' && page_table[i].isPageReferenced=='1'){

					int index_of_page_in_ram=page_table[i].pageNumberInRam ;

					if(page_table[i].isPageModified=='1'){

						statistics_for_threads[tNameindex][3]=statistics_for_threads[tNameindex][3]+1;
						fseek(fp,i*frame_size*4,SEEK_SET);

						fwrite(&pysicall_memory[index_of_page_in_ram*frame_size],sizeof(int),frame_size,fp); //increment number of disk page writes
					}	

					page_table[i].isPagePresent='0';
					page_table[index_of_page_in_virtual_memory].isPagePresent='1';

					page_table[index_of_page_in_virtual_memory].isPageModified='0';
					page_table[index_of_page_in_virtual_memory].isPageReferenced='0';

					page_table[index_of_page_in_virtual_memory].pageNumberInRam=index_of_page_in_ram;

					fseek(fp,index_of_page_in_virtual_memory*frame_size*4,SEEK_SET);

					fread(&pysicall_memory[index_of_page_in_ram*frame_size],sizeof(int),frame_size,fp);
					statistics_for_threads[tNameindex][2]=statistics_for_threads[tNameindex][2]+1; //increment number of disk page reads
					return 5;

				}

			}

			//if we cant not found a page that has data for related thread then  we apply page replacement like global

			//looking for a page that has class0 properties and it is in the pysicall memory(Present)
			for(int i=0;i<virtual_memory_frames_number;++i){

				if(page_table[i].isPagePresent=='1' && page_table[i].isPageModified=='0' && page_table[i].isPageReferenced=='0'){
					page_table[i].isPagePresent='0';

					int index_of_page_in_ram=page_table[i].pageNumberInRam ;

					if(page_table[i].isPageModified=='1'){

						statistics_for_threads[tNameindex][3]=statistics_for_threads[tNameindex][3]+1; //increment number of disk page writes
						fseek(fp,i*frame_size*4,SEEK_SET);

						fwrite(&pysicall_memory[index_of_page_in_ram*frame_size],sizeof(int),frame_size,fp);
					}	


					page_table[index_of_page_in_virtual_memory].isPagePresent='1';

					page_table[index_of_page_in_virtual_memory].isPageModified='0';
					page_table[index_of_page_in_virtual_memory].isPageReferenced='0';

					page_table[index_of_page_in_virtual_memory].pageNumberInRam=index_of_page_in_ram;

					fseek(fp,index_of_page_in_virtual_memory*frame_size*4,SEEK_SET);

					fread(&pysicall_memory[index_of_page_in_ram*frame_size],sizeof(int),frame_size,fp);

					statistics_for_threads[tNameindex][2]=statistics_for_threads[tNameindex][2]+1; //increment number of disk page reads
					return 5;

				}

			}

			//looking for a page that has class1 properties and it is in the pysicall memory(Present)
			for(int i=0;i<virtual_memory_frames_number;++i){

				if(page_table[i].isPagePresent=='1' && page_table[i].isPageModified=='1' && page_table[i].isPageReferenced=='0'){
					page_table[i].isPagePresent='0';

					int index_of_page_in_ram=page_table[i].pageNumberInRam ;

					if(page_table[i].isPageModified=='1'){
						statistics_for_threads[tNameindex][3]=statistics_for_threads[tNameindex][3]+1; //increment number of disk page writes
						fseek(fp,i*frame_size*4,SEEK_SET);

						fwrite(&pysicall_memory[index_of_page_in_ram*frame_size],sizeof(int),frame_size,fp);
					}	


					page_table[index_of_page_in_virtual_memory].isPagePresent='1';

					page_table[index_of_page_in_virtual_memory].isPageModified='0';
					page_table[index_of_page_in_virtual_memory].isPageReferenced='0';

					page_table[index_of_page_in_virtual_memory].pageNumberInRam=index_of_page_in_ram;

					fseek(fp,index_of_page_in_virtual_memory*frame_size*4,SEEK_SET);

					fread(&pysicall_memory[index_of_page_in_ram*frame_size],sizeof(int),frame_size,fp);

					statistics_for_threads[tNameindex][2]=statistics_for_threads[tNameindex][2]+1; //increment number of disk page reads
					return 5;

				}

			}
			//looking for a page that has class2 properties and it is in the pysicall memory(Present)
			for(int i=0;i<virtual_memory_frames_number;++i){

				if(page_table[i].isPagePresent=='1' && page_table[i].isPageModified=='0' && page_table[i].isPageReferenced=='1'){

					page_table[i].isPagePresent='0';

					int index_of_page_in_ram=page_table[i].pageNumberInRam ;

					if(page_table[i].isPageModified=='1'){
						statistics_for_threads[tNameindex][3]=statistics_for_threads[tNameindex][3]+1; //increment number of disk page writes
						fseek(fp,i*frame_size*4,SEEK_SET);

						fwrite(&pysicall_memory[index_of_page_in_ram*frame_size],sizeof(int),frame_size,fp);
					}	


					page_table[index_of_page_in_virtual_memory].isPagePresent='1';

					page_table[index_of_page_in_virtual_memory].isPageModified='0';
					page_table[index_of_page_in_virtual_memory].isPageReferenced='0';

					page_table[index_of_page_in_virtual_memory].pageNumberInRam=index_of_page_in_ram;

					fseek(fp,index_of_page_in_virtual_memory*frame_size*4,SEEK_SET);

					fread(&pysicall_memory[index_of_page_in_ram*frame_size],sizeof(int),frame_size,fp);

					statistics_for_threads[tNameindex][2]=statistics_for_threads[tNameindex][2]+1; //increment number of disk page reads
					return 5;

				}

			}
			//looking for a page that has class3 properties and it is in the pysicall memory(Present)
			for(int i=0;i<virtual_memory_frames_number;++i){

				if(page_table[i].isPagePresent=='1' && page_table[i].isPageModified=='1' && page_table[i].isPageReferenced=='1'){

					int index_of_page_in_ram=page_table[i].pageNumberInRam ;

					if(page_table[i].isPageModified=='1'){
						statistics_for_threads[tNameindex][3]=statistics_for_threads[tNameindex][3]+1; //increment number of disk page writes
						fseek(fp,i*frame_size*4,SEEK_SET);

						fwrite(&pysicall_memory[index_of_page_in_ram*frame_size],sizeof(int),frame_size,fp);
					}	

					page_table[i].isPagePresent='0';
					page_table[index_of_page_in_virtual_memory].isPagePresent='1';

					page_table[index_of_page_in_virtual_memory].isPageModified='0';
					page_table[index_of_page_in_virtual_memory].isPageReferenced='0';

					page_table[index_of_page_in_virtual_memory].pageNumberInRam=index_of_page_in_ram;

					fseek(fp,index_of_page_in_virtual_memory*frame_size*4,SEEK_SET);

					fread(&pysicall_memory[index_of_page_in_ram*frame_size],sizeof(int),frame_size,fp);
					statistics_for_threads[tNameindex][2]=statistics_for_threads[tNameindex][2]+1; //increment number of disk page reads
					return 5;

				}

			}


		}

		}

		else{

			statistics_for_threads[tNameindex][5]=statistics_for_threads[tNameindex][5]-1; //we increment this counter at the top. But if memory is not full yet then this means there is no page replacement("replacement")
			int counter=0;
			for(int i=0;i<virtual_memory_frames_number;++i){

				if(page_table[i].isPagePresent=='1')
					counter+=1;
			}

			fseek(fp,frame_size*index_of_page_in_virtual_memory*4,SEEK_SET);

			fread(&pysicall_memory[counter*frame_size],sizeof(int),frame_size,fp);

			statistics_for_threads[tNameindex][2]=statistics_for_threads[tNameindex][2]+1; //increment number of disk page reads

			page_table[index_of_page_in_virtual_memory].isPagePresent='1';

			page_table[index_of_page_in_virtual_memory].pageNumberInRam=counter;	
			return 5;
		}
	}           
}


void print_page_table(){

	if(memory_access_counter==print_table_in_each){

		memory_access_counter=0;

		cout<<"-------------------------------Page Table---------------------------------\n";

		cout<<"page_table_index:     isPagePresent(0 or 1)     isPageReferenced(0 or 1)     isPageModified(0 or 1)     page_index_in_pysicall_memory\n";

		for(int i=0;i<virtual_memory_frames_number;++i){

			cout<<i<<":  "<<page_table[i].isPagePresent<<"     "<<page_table[i].isPageReferenced<<"     "<<page_table[i].isPageModified<<"     "<<page_table[i].pageNumberInRam<<endl;

		}
	}
}


void reset_page_table(){

	if(memory_access_counter2==100){
		memory_access_counter2=0;

		for(int i=0;i<virtual_memory_frames_number;++i){

			page_table[i].isPageReferenced='0';
		}
	}

}


void set(unsigned int index, int value, char * tName){

std::unique_lock<std::mutex> locker(access_operation);


	print_page_table(); //print page table function

	if(strcmp(page_replacement_algorithm_name,"NRU")==0)
		reset_page_table();	//reset page_table according to on each number of memory reference

	memory_access_counter+=1;
	memory_access_counter2+=1;

	if(strcmp(tName,"fill")==0){

		int temp_value=value;
		memory_access_counter-=1;  //dont want "fill thread" to effect behaviour of regular programs(threads).
		memory_access_counter2-=1;

		fseek(fp,index*4,SEEK_SET);

		fwrite(&temp_value,sizeof(int),1,fp);


		statistics_for_threads[0][1]= statistics_for_threads[0][1]+1;


		return;
	}
	else if(strcmp(tName,"quick")==0){


		int page_table_index= index/(unsigned int)frame_size;

		struct page_table_entry temp_page_table_entry=page_table[page_table_index];
		statistics_for_threads[2][1]= statistics_for_threads[2][1]+1;



		if(temp_page_table_entry.isPagePresent=='1'){ //if data is in the memory

			page_table[page_table_index].instruction_counter_number=instruction_counter_total;

			int index_in_ram=(temp_page_table_entry.pageNumberInRam*frame_size)+(index%frame_size);

			pysicall_memory[index_in_ram]=value;

			page_table[page_table_index].isPageModified='1';

			instruction_counter_total+=1;
			return;

		}
		else{ //if data is not in the memory then

			statistics_for_threads[2][4]= statistics_for_threads[2][4]+1;
			//page replacement will be executed here

			
			page_replacement(page_table_index,tName,pageReplacement);

			page_table[page_table_index].instruction_counter_number=instruction_counter_total;
			temp_page_table_entry=page_table[page_table_index];

			

			int index_in_ram=(temp_page_table_entry.pageNumberInRam*frame_size)+(index%frame_size);

			pysicall_memory[index_in_ram]=value;

			page_table[page_table_index].isPageModified='1';
			instruction_counter_total+=1;

			return;
		}



	}

	else if(strcmp(tName,"bubble")==0){

		int page_table_index= index/(unsigned int)frame_size;

		struct page_table_entry temp_page_table_entry=page_table[page_table_index];

		statistics_for_threads[1][1]= statistics_for_threads[1][1]+1;
		if(temp_page_table_entry.isPagePresent=='1'){  //if data is in the memory

			page_table[page_table_index].instruction_counter_number=instruction_counter_total;
			int index_in_ram=(temp_page_table_entry.pageNumberInRam*frame_size)+(index%frame_size);

			pysicall_memory[index_in_ram]=value;

			page_table[page_table_index].isPageModified='1';
			instruction_counter_total+=1;
			return;

		}
		else{  //if data is not in the memory then

			statistics_for_threads[1][4]= statistics_for_threads[1][4]+1;
			//page replacement will be executed here

			
			page_replacement(page_table_index,tName,pageReplacement);
			page_table[page_table_index].instruction_counter_number=instruction_counter_total;
			temp_page_table_entry=page_table[page_table_index];

			
			int index_in_ram=(temp_page_table_entry.pageNumberInRam*frame_size)+(index%frame_size);

			pysicall_memory[index_in_ram]=value;

			page_table[page_table_index].isPageModified='1';
			instruction_counter_total+=1;

			return;
		}

	}

	else if(strcmp(tName,"merge")==0){


		int page_table_index= index/(unsigned int)frame_size;

		struct page_table_entry temp_page_table_entry=page_table[page_table_index];

		statistics_for_threads[3][1]= statistics_for_threads[3][1]+1;
		if(temp_page_table_entry.isPagePresent=='1'){  //if data is in the memory

			page_table[page_table_index].instruction_counter_number=instruction_counter_total;
			int index_in_ram=(temp_page_table_entry.pageNumberInRam*frame_size)+(index%frame_size);

			pysicall_memory[index_in_ram]=value;

			page_table[page_table_index].isPageModified='1';

			instruction_counter_total+=1;
			return;

		}

		else{ //if data is not in the memory then

			statistics_for_threads[3][4]= statistics_for_threads[3][4]+1;
			//page replacement will be executed here

			
			page_replacement(page_table_index,tName,pageReplacement);
			page_table[page_table_index].instruction_counter_number=instruction_counter_total;
			temp_page_table_entry=page_table[page_table_index];

			
			int index_in_ram=(temp_page_table_entry.pageNumberInRam*frame_size)+(index%frame_size);

			pysicall_memory[index_in_ram]=value;

			page_table[page_table_index].isPageModified='1';
			instruction_counter_total+=1;

			return;
		}



	}


	else if(strcmp(tName,"index")==0){

		int page_table_index= index/(unsigned int)frame_size;

		struct page_table_entry temp_page_table_entry=page_table[page_table_index];
		statistics_for_threads[4][1]= statistics_for_threads[4][1]+1;
		if(temp_page_table_entry.isPagePresent=='1'){ //if data is in the memory

			page_table[page_table_index].instruction_counter_number=instruction_counter_total;
			int index_in_ram=(temp_page_table_entry.pageNumberInRam*frame_size)+(index%frame_size);

			pysicall_memory[index_in_ram]=value;

			page_table[page_table_index].isPageModified='1';

			instruction_counter_total+=1;
			return;

		}

		else{ //if data is not in the memory then

			statistics_for_threads[4][4]= statistics_for_threads[4][4]+1;
			//page replacement will be executed here

			
			page_replacement(page_table_index,tName,pageReplacement);
			page_table[page_table_index].instruction_counter_number=instruction_counter_total;
			temp_page_table_entry=page_table[page_table_index];

			
			int index_in_ram=(temp_page_table_entry.pageNumberInRam*frame_size)+(index%frame_size);

			pysicall_memory[index_in_ram]=value;

			page_table[page_table_index].isPageModified='1';

			instruction_counter_total+=1;


			return;
		}


	}


	else{

		memory_access_counter-=1;
		memory_access_counter2-=1;
		return;
	}


}


int get(unsigned int index, char * tName){

std::unique_lock<std::mutex> locker(access_operation);

	print_page_table(); //print page table 

	if(strcmp(page_replacement_algorithm_name,"NRU")==0)
		reset_page_table(); //reset page_table according to on each number of memory reference
	memory_access_counter+=1;
	memory_access_counter2+=1;

	if(strcmp(tName,"fill")==0){ //fill thread never gonna call get function so return dumply

		memory_access_counter-=1;
		memory_access_counter2-=1;
		return -2;

	}

	else if(strcmp(tName,"quick")==0){

		statistics_for_threads[2][0]= statistics_for_threads[2][0]+1;


		int page_table_index= index/(unsigned int)frame_size;

		struct page_table_entry temp_page_table_entry=page_table[page_table_index];

		if(temp_page_table_entry.isPagePresent=='1'){ //if data is in the memory

			page_table[page_table_index].instruction_counter_number=instruction_counter_total;
			int index_in_ram=(temp_page_table_entry.pageNumberInRam*frame_size)+(index%frame_size);
			page_table[page_table_index].isPageReferenced='1';
			instruction_counter_total+=1;
			return pysicall_memory[index_in_ram];

		}

		else{ //if data is not in the memory then

			statistics_for_threads[2][4]= statistics_for_threads[2][4]+1;
			page_replacement(page_table_index,tName,pageReplacement);
			page_table[page_table_index].instruction_counter_number=instruction_counter_total;
			//page replacement will be executed here

			temp_page_table_entry=page_table[page_table_index];

			int index_in_ram=(temp_page_table_entry.pageNumberInRam*frame_size)+(index%frame_size);

			page_table[page_table_index].isPageReferenced='1';
			instruction_counter_total+=1;
			return pysicall_memory[index_in_ram];

		}
		
	}


	else if(strcmp(tName,"bubble")==0){

		statistics_for_threads[1][0]= statistics_for_threads[1][0]+1;


		int page_table_index= index/(unsigned int)frame_size;

		struct page_table_entry temp_page_table_entry=page_table[page_table_index];

		if(temp_page_table_entry.isPagePresent=='1'){  //if data is in the memory

			page_table[page_table_index].instruction_counter_number=instruction_counter_total;
			int index_in_ram=(temp_page_table_entry.pageNumberInRam*frame_size)+(index%frame_size);
			page_table[page_table_index].isPageReferenced='1';
			instruction_counter_total+=1;
			return pysicall_memory[index_in_ram];

		}

		else{  //if data is not in the memory then

			statistics_for_threads[1][4]= statistics_for_threads[1][4]+1;
			page_replacement(page_table_index,tName,pageReplacement);
			page_table[page_table_index].instruction_counter_number=instruction_counter_total;
			//page replacement will be executed here
			
			temp_page_table_entry=page_table[page_table_index];

			int index_in_ram=(temp_page_table_entry.pageNumberInRam*frame_size)+(index%frame_size);

			page_table[page_table_index].isPageReferenced='1';
			instruction_counter_total+=1;
			return pysicall_memory[index_in_ram];

		}

	}


	else if(strcmp(tName,"merge")==0){

		statistics_for_threads[3][0]= statistics_for_threads[3][0]+1;


		int page_table_index= index/(unsigned int)frame_size;

		struct page_table_entry temp_page_table_entry=page_table[page_table_index];

		if(temp_page_table_entry.isPagePresent=='1'){ //if data is in the memory

			page_table[page_table_index].instruction_counter_number=instruction_counter_total;
			int index_in_ram=(temp_page_table_entry.pageNumberInRam*frame_size)+(index%frame_size);
			page_table[page_table_index].isPageReferenced='1';
			instruction_counter_total+=1;
			return pysicall_memory[index_in_ram];

		}

		else{ //if data is not in the memory then

			statistics_for_threads[3][4]= statistics_for_threads[3][4]+1;
			page_replacement(page_table_index,tName,pageReplacement);
			page_table[page_table_index].instruction_counter_number=instruction_counter_total;
			//page replacement will be executed here
			
			temp_page_table_entry=page_table[page_table_index];

			int index_in_ram=(temp_page_table_entry.pageNumberInRam*frame_size)+(index%frame_size);

			page_table[page_table_index].isPageReferenced='1';
			instruction_counter_total+=1;
			return pysicall_memory[index_in_ram];
		}

		
	}	

	else if(strcmp(tName,"index")==0){

		statistics_for_threads[4][0]= statistics_for_threads[4][0]+1;

		int page_table_index= index/(unsigned int)frame_size;

		struct page_table_entry temp_page_table_entry=page_table[page_table_index];

		if(temp_page_table_entry.isPagePresent=='1'){  //if data is in the memory

			page_table[page_table_index].instruction_counter_number=instruction_counter_total;
			int index_in_ram=(temp_page_table_entry.pageNumberInRam*frame_size)+(index%frame_size);
			page_table[page_table_index].isPageReferenced='1';
			instruction_counter_total+=1;
			return pysicall_memory[index_in_ram];

		}

		else{ //if data is not in the memory then
			
			statistics_for_threads[4][4]= statistics_for_threads[4][4]+1;
			page_replacement(page_table_index,tName,pageReplacement);
			page_table[page_table_index].instruction_counter_number=instruction_counter_total;
			//page replacement will be executed here
			temp_page_table_entry=page_table[page_table_index];
			
			int index_in_ram=(temp_page_table_entry.pageNumberInRam*frame_size)+(index%frame_size);

			page_table[page_table_index].isPageReferenced='1';

			instruction_counter_total+=1;
			return pysicall_memory[index_in_ram];
		}

		
	}	

	else{

		memory_access_counter-=1;
		memory_access_counter2-=1;

		statistics_for_threads[5][0]= statistics_for_threads[5][0]+1;

		int page_table_index= index/frame_size;

		struct page_table_entry temp_page_table_entry=page_table[page_table_index];

		if(temp_page_table_entry.isPagePresent=='1'){

			if(temp_page_table_entry.isPageModified=='1'){


				fseek(fp,page_table_index*frame_size*4,SEEK_SET);
				fwrite(&pysicall_memory[temp_page_table_entry.pageNumberInRam*frame_size],sizeof(int),frame_size,fp);
				page_table[page_table_index].isPagePresent='0';

			}
			int index_in_ram=(temp_page_table_entry.pageNumberInRam*frame_size)+(index%frame_size);
			page_table[page_table_index].isPageReferenced='1';
			return pysicall_memory[index_in_ram];

		}

		else{

		//	statistics_for_threads[5][4]= statistics_for_threads[5][4]+1;  //page replacements increment
		//	statistics_for_threads[5][5]= statistics_for_threads[5][5]+1;  //page replacements increment

		//	statistics_for_threads[5][2]= statistics_for_threads[5][2]+1;  //disk page reads
		//	statistics_for_threads[5][3]= statistics_for_threads[5][3]+1;  //disk page writes

			int tempint;
			fseek(fp,page_table_index*frame_size*4+(index%frame_size)*4,SEEK_SET);

			fread(&tempint,sizeof(int),1,fp);

			return tempint;
		}

	}
	
}

void merge(int start_index, int l, int m, int r) 
{ 

	char program_name[20] ="merge";
    int i, j, k; 
    int n1 = m - l + 1; 
    int n2 = r - m; 
  
    int L[n1], R[n2]; 
  
    for (i = 0; i < n1; i++) 

    	
        L[i] = get((start_index+l+i),program_name); 
    for (j = 0; j < n2; j++) 
        R[j] = get((start_index+m+j+1),program_name); 
  
    i = 0; 
    j = 0;  
    k = l; 
    while (i < n1 && j < n2) { 
        if (L[i] <= R[j]) { 

        	set((start_index+k),L[i],program_name);
            i++; 
        } 
        else { 
        	set((start_index+k),R[j],program_name);
            j++; 
        } 
        k++; 
    } 
  
    while (i < n1) { 

    	set((start_index+k),L[i],program_name);

        i++; 
        k++; 
    } 
  
    while (j < n2) { 
    	set((start_index+k),R[j],program_name);
        j++; 
        k++; 
    } 
} 
  

void mergeSort(int start_index, int l, int r) 
{ 
    if (l < r) { 

        int m = l + (r - l) / 2; 
   
        mergeSort(start_index, l, m); 
        mergeSort(start_index, m + 1, r); 
  
        merge(start_index, l, m, r); 
    } 
} 

void bubbleSort(int start_index, int size)  
{  

	char program_name[20]="bubble";
    int i, j;  
    for (i = 0; i < size-1; i++) {     
      
    for (j = 0; j < size-i-1; j++)  {
        if ( get((start_index+j),program_name) > get((start_index+j+1),program_name) )  {
        	int temp= get((start_index+j),program_name);

        	set(start_index+j,get((start_index+j+1),program_name),program_name);

        	set(start_index+j+1,temp,program_name); 

        }
       }
   }
}

int part_select(int start_index, int start, int end){

	char program_name[20]="quick";

	int i= (start-1);

	int pivot_element= get(start_index+end,program_name);

	for (int k=start; k<end;++k){

		if(get(start_index+k,program_name)<= pivot_element){
			++i;
			int temp=get(start_index+i,program_name);
			set(start_index+i,get(start_index+k,program_name),program_name);
			set(start_index+k,temp,program_name);
		}
	}
	int temp=get(start_index+i+1,program_name);
	set(start_index+i+1,get(start_index+end,program_name),program_name);
	set(start_index+end,temp,program_name);	

	return (i+1);
}

void quicksort(int start_index,int  start,int end){

	if(start<end){

	int pivot_element=part_select(start_index,start,end);

	quicksort(start_index,start,pivot_element-1);

	quicksort(start_index,pivot_element+1,end);

	}
	return;
}


void indexsort(int start_index, int size){

	char program_name[20]="index";

	int i;
	int important_element;
	int j;

	for(int i=0;i<size;++i){

		important_element= get(start_index+i,program_name);

		j=i-1;

		for(;j>=0 && get(start_index+j,program_name) > important_element;){

			set(start_index+j+1,get(start_index+j,program_name),program_name);
			j=j-1;
		}

		set(start_index+j+1,important_element,program_name);

	}
}

void timer(){

	int i=0;
	while(i<10000){


		this_thread::sleep_for(std::chrono::milliseconds(50));

		for(int i=0;i<virtual_memory_frames_number;++i){

			page_table[i].isPageReferenced='0';
		}
		++i;
	}

}

int check(){

char program_name[20]="check";

for(int i=0;i<frame_size*virtual_memory_frames_number;){

	int maks=get((unsigned int)(i),program_name);

	//cout<<maks<<endl;

	for(int j=1;j<((frame_size*virtual_memory_frames_number)/4);++j){

		int temp2= get((unsigned int)(i+j),program_name);

		//cout<<temp2<<endl;

		if(temp2<maks)
			return -1;
		else
			maks=temp2;
	}


	i=i+((frame_size*virtual_memory_frames_number)/4);

}
return 1;
}

int main(int argc,char** argv){

	//check parameters
	//|| atoi(argv[3])< atoi(argv[2])

	if(argc!=8){
		cout<<"Wrong parameter to program\n";

		return 0;
	}

	if(atoi(argv[1])<0 || atoi(argv[2])<0 || atoi(argv[6])<1){
		cout<<"check parameters! you enter a wrong parameter. Think again :(\n";
		return 0;
	}

	if(strcmp(argv[4],"WSClock")==0){
		cout<<"This algorithm is not supported , sorry\n";
		return 0;
	}

	if(strcmp(argv[4],"FIFO")!=0 && strcmp(argv[4],"NRU")!=0 && strcmp(argv[4],"LRU")!=0 && strcmp(argv[4],"FIFO")==0){

		cout<<"There is no such a algorithm, sorry\n";
		return 0;
	}

	if(strcmp(argv[5],"local")!=0 && strcmp(argv[5],"global")!=0){
		cout<<"There is no such a policy\n";
		return 0;
	}

	
	for(int i=0;i<6;++i){ //initial of statistics array

		for(int j=0;j<6;++j){

			statistics_for_threads[i][j]=0;
		}
	}


	frame_size= pow(2.0,atoi(argv[1]));

	pysicall_memory_frames_number=pow(2.0,atoi(argv[2]));

	virtual_memory_frames_number=pow(2.0,atoi(argv[3]));

	//memory allocation for page_table
	page_table=(struct page_table_entry *)malloc(sizeof(struct page_table_entry)*virtual_memory_frames_number);

	//memory allocation for pysicall memory
	pysicall_memory=(int *)malloc(sizeof(int)*frame_size*pysicall_memory_frames_number);


	page_replacement_algorithm_name=(char*)malloc(sizeof(char)*20);
	pageReplacement=(char*)malloc(sizeof(char)*20);


	strcpy(page_replacement_algorithm_name,argv[4]);

	strcpy(pageReplacement,argv[5]);

	print_table_in_each=atoi(argv[6]);

	//open disk file
	fp=fopen(argv[7],"wb+");


	char program_name[10]="fill";   //bunu degistir

	srand(1000);

	int random_number;	
	for(int i=frame_size*virtual_memory_frames_number;i>0;--i){
		random_number=rand();
		//random_number=i;

		set(frame_size*virtual_memory_frames_number-i,random_number,program_name);
		//fwrite(&random_number,sizeof(int),1,fp);
	}
	
	//starting threads for sorting

	std::thread t(mergeSort, ((frame_size*virtual_memory_frames_number)/4)*2, 0, (frame_size*virtual_memory_frames_number)/4-1);

	std::thread t1(bubbleSort, 0, (frame_size*virtual_memory_frames_number)/4);

	std::thread t2(quicksort,((frame_size*virtual_memory_frames_number)/4),0,(frame_size*virtual_memory_frames_number)/4-1);

	std::thread t3(indexsort,((frame_size*virtual_memory_frames_number)/4)*3,(frame_size*virtual_memory_frames_number)/4);


	//wait threads to finish
	t.join();

	t1.join();  

	t2.join();

	t3.join();

	
	//check that each sorting threads sorted file correctly
	int sort_correct=check();

	if(sort_correct==-1){
		cout<<"Sorting is not succesfull\n";
	}
	else{
		cout<<"Sorting is succesfull\n";
	}

fclose(fp);


	//print statistics for threads

	for(int i=0;i<6;++i){

		if(i==0)
			cout<<"Fill     :";

		else if(i==1)
			cout<<"Bubble    :";
		else if(i==2)
			cout<<"Quick     :";
		else if(i==3)
			cout<<"Merge     :";
		else if(i==4)
			cout<<"Index     :";

		else if(i==5)
			cout<<"Check     :";

		for(int j=0; j<6;++j){

			cout<<statistics_for_threads[i][j]<<"  ";
		}

		cout<<"\n";
	}	        

	return 0;
}
