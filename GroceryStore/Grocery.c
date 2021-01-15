#include <stdio.h>
#include <string.h>
#include <stdlib.h>


/*This code WILL run and execute on OnlineGDB's c online compiler, please make sure the correct .dat files are open 
if you are testing this code on OnlineGDB's c online compiler using the compilers file open tab, thankyou*/

//////////////////STRUCTS//////////////////////
typedef struct ItemStruct {
	int ItemCode;
	int NumOfItems;
	float ItemPrice;
	float TotalItemSales;
	char ItemName[20];
	struct ItemStruct *next;
}Item;

typedef struct CategoryStruct{
	int CategoryCode;
	char CategoryName[20];
	int NumOfItems;
	float TotalCatSales;
	Item *head;	
}Category;


////////////////Prototypes///////////////////////

//this function is used to initilize the linked ITEM lists from the CodeNamePrice.dat file (case1)
void initilizeLists(Category *CategoryArray, FILE *CNPptr2);

//this function is used to initilize the  CATEGORY struct array from the CategoryName.dat file, this struct array has a memeber that is the head of the ITEM linked lists (case2)
void initilizeCategroies(Category *CategoryArray, FILE *CNptr);

//this function introduces a seprate linked list, of struct ITEM, that houses the individual 
int initilizeCustomerList(FILE *DTptr, Category *CategoryArray);

//used inside the initilizeCustomerList function to search for the price of a product given its key
float findItemPrice(Category *CategoryArray,int key);

//used inside the initilizeCustomerList function to search for the name of a product given its key
const char *findName(Category *CategoryArray,int key);

//used inside the initilizeCustomerList function to update the ITEM list totalItemsales and NumofItems members iin preperation for outputting the Inventory Report
void updateListStats (int key, int NUMITEMS, float TOTALSALES, Category *CategoryArray);

//takes in the head of a linked list and takes apart and deallocates the list
void deAllocateList(Item *source);

//Inventory report, data read from the ITEM linked lists, here we also update the CATEGORY struct array memebers for total sales and total items in category in preperation for the daily report
//data here is updated in the updateListStats call in the initilizeCustomerList function call!!!
void InventoryReport(Category *CategoryArray);

//used to create the appropriate file name for the inventoryXXX.dat files
FILE *createFileName(char code[]);

//by accessing the CATEGORY STRUCT ARRAY (CATEGORYARRAY) we output the daily sales report (the CategoryArray members should be updated after the InventoryReport function)
void DailySalesReport(int NumCustomer, Category *CategoryArray);

//aux function for initilizing the LINKED ITEM LIST
//NOTE!! I assume the data in CodeNamePrice.dat will come in the form given to us with a integer, string, then float!!
int getFileLineLength();

//used to help format the string data from CategoryName.dat
//NOTE!! I assume the data in CategoryName.dat will be in the form of integer, string!!
void removewhiteSpaces(char returnArray[], char inputArray[]);

//////////////////Functions////////////////////

//used un initilizeLists to help create the linked ITEM list
//NOTE!! I assume the data in CodeNamePrice.dat will come in the form given to us with a integer, string, then float!!
int getFileLineLength(){
    FILE *checkFile=NULL;
    int var1,counter=0;
    float var2;
    char vars3[20];
    checkFile=fopen("CodeNamePrice.dat", "r");
    
    if (checkFile==NULL){
    printf ("!!There's an ERROR with the file!!!");
    exit(1);
}
    
    while(!feof(checkFile)){
        fscanf (checkFile, "%d", &var1);
        fscanf (checkFile, " %17[^\t]", vars3);
        fscanf (checkFile, "%f", &var2);
        counter++;
    }
    fclose(checkFile);
    return counter;
    
}

//creating Linked Item List
void initilizeLists(Category *CategoryArray, FILE *CNPptr2){
    CNPptr2=fopen("CodeNamePrice.dat", "r");
    
    if (CNPptr2==NULL){
    printf ("!!There's an ERROR with the file!!!");
    exit(1);
    }
    int n;
    n=getFileLineLength();

	
    while (!feof(CNPptr2)){
        for (int i=0; i<n; i++){
            int CatCode=0,ItemCodeVar=0;
            float ItemPriceVar=0;
            //allocate memory for new node
            Item *nnode= (Item*)malloc(sizeof(Item));
            
            //scan in from file to Item nodes
            /*NOTE! I assume that there will be a \t inbetween the item name
            and price in the CodeNamePrice.dat file*/
            fscanf (CNPptr2, "%d", &ItemCodeVar);
            fscanf (CNPptr2, " %17[^\t]", nnode->ItemName);
            fscanf (CNPptr2, "%f", &nnode->ItemPrice);
            nnode->ItemCode=ItemCodeVar;
            nnode->NumOfItems=0;
            nnode->TotalItemSales=0.00;
            
        
            //finds what category index to use
            CatCode=(ItemCodeVar/100)-1;
            
            if (CategoryArray[CatCode].head==NULL){
                CategoryArray[CatCode].head=nnode;
            }
            else{
                nnode->next=CategoryArray[CatCode].head;
                CategoryArray[CatCode].head=nnode;
            }
           
          }
    }
    fclose(CNPptr2);
    return;
}

//used to help format the string data from CategoryName.dat
//NOTE!! I assume the data in CategoryName.dat will be in the form of integer then a string!!
void removewhiteSpaces(char returnArray[], char inputArray[]){
    int d=0;
    for (int i=0; inputArray[i]!='\0'; i++){
        if ((inputArray[i]>=65 && inputArray[i]<=90)||(inputArray[i]>=97&& inputArray[i]<=122)){
        returnArray[d]=inputArray[i];
        d++;
        }
    }
    returnArray[d]='\0';
    return;
}

//Creating the category
void initilizeCategroies(Category *CategoryArray, FILE *CNptr){
    
    CNptr=fopen("CategoryName.dat", "r");
    if (CNptr==NULL){
        printf ("!!There's an ERROR with the file!!!");
        exit(1);
    }
    //arrays used for removewhiteSpaces function
    //assuming that strings will be no longer than 20 characters (Its stated that they are no longer than 16 however)
    char returnArray[20], inputArray[20];
    
    while (!feof(CNptr)){
	
	for (int i=0; i<8; i++){
        //iterate through the CategoryArray struct array, initilize each category struct with the  data in the file
        fscanf (CNptr,"%d", &CategoryArray[i].CategoryCode);
		fscanf (CNptr, "%[^\n]s", inputArray);
		removewhiteSpaces(returnArray,inputArray);
		strcpy(CategoryArray[i].CategoryName,returnArray);
		CategoryArray[i].NumOfItems=0;
		CategoryArray[i].TotalCatSales=0.00;
		CategoryArray[i].head=NULL;
	    }	
	}
	fclose(CNptr);
	return;

}

//finding the item price
float findItemPrice(Category *CategoryArray, int key){
    //using the 3 digit code find which array index to use
    int i=(key/100)-1;
    Item *source;
    source=CategoryArray[i].head;
    //search through linked list until you find matching keys, then return value
    for ( ; source!=NULL; source=source->next){
        if (source->ItemCode==key){
            return source->ItemPrice;
        }
    }
    //the function should never return this value
    return 1.23;
}

const char *findName(Category *CategoryArray,int key){
    char character='A';
    const char *ptr =&character;
    //using the 3 digit code find which array index to use
    int i=(key/100)-1;
    Item *source;
    source=CategoryArray[i].head;
    //search through linked list until you find matching keys, then return value
    for ( ; source!=NULL; source=source->next){
        if (source->ItemCode==key){
            return source->ItemName;
        }
    }
    //the function should never return this value
    return ptr;
}

//this function updates the neccessary data structures (ITEM linked lists) in preperation for the inventory report
void updateListStats (int key, int NUMITEMS, float TOTALSALES, Category *CategoryArray){
    //search for key, find the key, and update the appropriate node with the passed in values
    int i=(key/100)-1;
    Item *source;
    source=CategoryArray[i].head;
    for ( ; source!=NULL; source=source->next){
        if (source->ItemCode==key){
            source->NumOfItems+=NUMITEMS;
            source->TotalItemSales+=TOTALSALES;
            
        }
    }
    return;
}

//function used to deallocate a linked list
//repeats deleting end node, (works from end to front)
void deAllocateList( Item *source){
    //if the linked list is empty
    if(source==NULL){
        return;
    }
    
    while (source->next!=NULL){
        Item *previous= NULL;
        Item *current= source;
        while (current->next!=NULL){
            previous= current;
            current= current->next;
        }
        previous->next=NULL;
        free(current);}
    free(source);
    return;
}

int initilizeCustomerList(FILE *DTptr, Category *CategoryArray){
    
    DTptr=fopen("DailyTransactions.dat", "r");
    if (DTptr==NULL){
    printf ("!!There's an ERROR with the file!!!");
    exit(1);
}
    int NumOfCustomers=0, code, num;
    Item *ncurrent=NULL;
    Item *precurrent=NULL;
    Item *ncurrentSORT=NULL;
    Item *ncurrentUPDT=NULL;
    Item *ncurrentPRINT=NULL;
    
    /*creating a new customer ITEM linked list to hold the data for their receipt
    //will continue until there is still receipt data, deallocating and deleting 
    the previous customer ITEM linked list each time in preperation for a new one*/
    
    /*NOTE!! I assume that each 000 will be after at least one item(code then number of items)
    in the DailyTransactions.dat file, and after the final 000 delimetter there wont be any whitespaces
    or newlines*/
    while (!feof(DTptr)){
        float totalReceiptSales=0.00;
        int totalReceiptItems=0;
        NumOfCustomers++;
        Item *ncustomer=(Item*)malloc(sizeof(Item));
        ncurrent=ncustomer;
        fscanf (DTptr,"%d",&code);
        fscanf (DTptr,"%d",&num);
        ncurrent->ItemCode= code;
        ncurrent->NumOfItems= num;
        fscanf(DTptr,"%d",&code);
        //will continue to create new nodes for the list until the 000 is read
        while(code!=0){
            fscanf(DTptr,"%d",&num);
            precurrent=ncurrent;
            ncurrent=(Item*)malloc(sizeof(Item));
            precurrent->next=ncurrent;
            ncurrent->NumOfItems=num;
            ncurrent->ItemCode=code;
            fscanf(DTptr,"%d",&code);
        }
            ncurrent->next=NULL;
        
        //sorting the items in customer list by moving values stored in the members
        int temp;
        ncurrentSORT=ncustomer;
        for ( ;ncurrentSORT->next!=NULL; ncurrentSORT=ncurrentSORT->next){
            for (Item *newcurrent=ncurrentSORT->next; newcurrent!=NULL; newcurrent=newcurrent->next){
                if (newcurrent->ItemCode < ncurrentSORT->ItemCode){
                    //for code
                    temp=newcurrent->ItemCode;
				    newcurrent->ItemCode=ncurrentSORT->ItemCode;
				    ncurrentSORT->ItemCode=temp;
				    //for items num
				        temp=newcurrent->NumOfItems;
				        newcurrent->NumOfItems=ncurrentSORT->NumOfItems;
				        ncurrentSORT->NumOfItems=temp;
				
                    }
                }
            }
        //finding the key in the other  linked list, and updating the customer linked list with the data
        
        ncurrentUPDT=ncustomer;
        while(ncurrentUPDT!=NULL){
            
            ncurrentUPDT->ItemPrice= findItemPrice(CategoryArray, ncurrentUPDT->ItemCode);
            strcpy(ncurrentUPDT->ItemName,findName(CategoryArray, ncurrentUPDT->ItemCode));
            ncurrentUPDT->TotalItemSales= (float) (ncurrentUPDT->NumOfItems)*(ncurrentUPDT->ItemPrice);
            totalReceiptItems= totalReceiptItems+ (ncurrentUPDT->NumOfItems);
            totalReceiptSales= totalReceiptSales+ (ncurrentUPDT->TotalItemSales);
            //updating the global linked list, (the original) this will help in case 5; and 6
            updateListStats(ncurrentUPDT->ItemCode, ncurrentUPDT->NumOfItems, ncurrentUPDT->TotalItemSales, CategoryArray);
            ncurrentUPDT= ncurrentUPDT->next;
        }
        
        //printing the customer receipt

        printf("Customer Receipt # %d\n", NumOfCustomers);
        printf("Code      Item Name        Price   NumItem        TotSale\n");
        
        ncurrentPRINT=ncustomer;
        while (ncurrentPRINT!=NULL){
            printf (" %d", ncurrentPRINT->ItemCode);
            printf ("      %-16s", ncurrentPRINT->ItemName);
            printf (" %5.2f", ncurrentPRINT->ItemPrice);
            printf ("   %7d", ncurrentPRINT->NumOfItems);
            printf ("   %10.2f\n", ncurrentPRINT->TotalItemSales);
            ncurrentPRINT=ncurrentPRINT->next;
        }
        printf("\n");
        printf("                              Totals:   %d       %5.2f\n", totalReceiptItems, totalReceiptSales);
        
        //deallocate customer string
        deAllocateList(ncustomer);
    }   
        
        
    fclose(DTptr); 
    return NumOfCustomers;
}

//used to create the specfic INVENTORYXXX filenames 
FILE *createFileName(char code[]){ //after concat filestart will be file name!!!
    char filestart[20]="Inventory";
    char ending[5]=".dat";
    strcat(filestart,code);
    strcat(filestart,ending);
    return fopen(filestart, "wb");
}

//prints the inventory report (data should be updated)
void InventoryReport(Category *CategoryArray){
    
    Item *current=NULL;
    char outPutStorage[100];
    FILE* IRptr[8];
    
    //goes through the Category Struct Array, printing out the data from its member ITEM linked list
    //as well as the totals of each Category
    for (int i=0; i<8; i++){
        int NumOfItemsCount=0;
        float TotalSalesCount=0;
        current=CategoryArray[i].head;
        sprintf(outPutStorage,"%4d", CategoryArray[i].CategoryCode);
        IRptr[i]= createFileName(outPutStorage);
        
        if (IRptr[i]==NULL){
            printf ("!!There's an ERROR with the file!!!");
            exit(1);
        }
        
        printf("\nCategory Name: %s\n",CategoryArray[i].CategoryName);
        printf("Category Code: %d\n",CategoryArray[i].CategoryCode);
        printf("Code\t Item Name\t NumItems\t Totsale\n");
        
        //goes though the categories linked list and outputs the data as strings in order to write to the binary files
        while(current!=NULL){
            //first changes the value to a string
            sprintf(outPutStorage,"%4d",current->ItemCode);
            //prints the string to stdout
            printf("%s", outPutStorage);
            //writes to the binary file using the string
            fwrite(outPutStorage,sizeof(char),sizeof(outPutStorage),IRptr[i]);
            sprintf(outPutStorage,"%-16s", current->ItemName);
            printf("     %s", outPutStorage);
            fwrite(outPutStorage,sizeof(char),sizeof(outPutStorage),IRptr[i]);
        
            sprintf(outPutStorage,"%4d", current->NumOfItems);
            printf("%s", outPutStorage);
            fwrite(outPutStorage,sizeof(char),sizeof(outPutStorage),IRptr[i]);
            NumOfItemsCount+=current->NumOfItems; //preparing to update the total num of items category, in category struct
        
            sprintf(outPutStorage,"%10.2f", current->TotalItemSales);
            printf("        %s", outPutStorage);
            //printf(" %f", current->TotalItemSales);
            fwrite(outPutStorage,sizeof(char),sizeof(outPutStorage),IRptr[i]);
            TotalSalesCount+=current->TotalItemSales; //preparing to update the total sales in category struct
    
            printf("\n");
            current=current->next;
        }
        
        printf("\nTotal Items Sold: %d\n",NumOfItemsCount);
        printf("Total Sales: %.2f\n",TotalSalesCount);
        //updating the category structs in preperation of the daily summary report
        CategoryArray[i].NumOfItems=NumOfItemsCount;
        CategoryArray[i].TotalCatSales=TotalSalesCount;
        fclose(IRptr[i]);
    }
    return;
    
}
//function to print the daily report
void DailySalesReport(int NumCustomer, Category *CategoryArray){
    int DailyTotalProductsSold=0;
    float DailySales=0.00;
    
    printf("\n\nDaily Summary Report\n");
    printf("Code\t    Category Name\t #Items Sold\t Tot Sales Amt\n");
    
    //cycles through the Category struct array printing the member data for each category
    //for the Daily Sales Report
    for (int i=0;i<8;i++){
     
        printf("%4d", CategoryArray[i].CategoryCode);
        printf("        %-16s", CategoryArray[i].CategoryName);
        printf("        %4d", CategoryArray[i].NumOfItems);
        DailyTotalProductsSold+=CategoryArray[i].NumOfItems;
        printf("          %.2f", CategoryArray[i].TotalCatSales);
        DailySales+=CategoryArray[i].TotalCatSales;
        printf("\n\n");
    }
    printf ("Total Customers  %8d\n", NumCustomer);
    printf ("Total Items Sold %8d\n", DailyTotalProductsSold);
    printf ("Total Sales      %8.2f\n\n", DailySales);
    
    return;
}

////////////////////MAIN//////////////////////////
int main(){
	
	//variables
	FILE *CNptr=NULL, *CNPptr2=NULL, *DTptr=NULL;
	int CustomerNum=0;
	
	
	struct CategoryStruct *CategoryArray;
	CategoryArray= (Category*)calloc(8,sizeof(Category));
    //initilizes category array
    initilizeCategroies(CategoryArray, CNptr);
    //initilizes the ITEM linked lists
	initilizeLists(CategoryArray, CNPptr2);
	//initilizeCustomerList returns how many customers were proccessed
	CustomerNum=initilizeCustomerList(DTptr, CategoryArray);
	InventoryReport(CategoryArray);
	DailySalesReport(CustomerNum, CategoryArray);
	
	//deallocating lists
	for (int i=0; i<8; i++){
	    deAllocateList(CategoryArray[i].head);
	}
	
//end
return 0;
}
	
    

	
	
	
        
