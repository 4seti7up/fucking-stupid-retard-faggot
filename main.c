#include <stdio.h>
#include <stdlib.h>
#include <malloc.h>
#include <string.h>
#include <time.h>

typedef struct {
    int dia,mes,year;
}fecha;

typedef struct {
    int temp_saldo,id_movimiento,id_origen,cuenta_destino;
    char tipo_transaccion[15];
    fecha fecha;
}Transferencia;


typedef struct {
    int id,id_usuario, saldo;
    fecha fecha;
}Cuentascliente;

typedef struct {
    int id;
    char nom[100];
    char apm[100];
    char app[100];
    fecha fecha;
}Clientes;

//----------------------------------Opcion movimientos
void deposito(char db_path[256],char db_acount[256]){
    FILE *transfer_db;
    FILE *acount_db;
    FILE *temporal;
    transfer_db=fopen(db_path,"r+b");
    acount_db=fopen(db_acount,"rb");
    temporal=fopen("TempT.dat","w+b");
    Cuentascliente reg, p;
    Transferencia dat, o;
    int n=0,bandera=0;
    printf("Ingrese Id_cuenta: ");
    scanf("%d", &n);
    while (fread(&reg, sizeof(Cuentascliente),1,acount_db)){
        if(reg.id==n){
            printf("Ingresa el monto a depositar: \n");
            scanf("%d", &o.temp_saldo);
            reg.saldo+=o.temp_saldo;
            fwrite(&reg, sizeof(Cuentascliente), 1, temporal);
            bandera++;
            int id = 0;
            char h[15]="Deposito";
            while (fread(&dat, sizeof(Transferencia), 1,transfer_db )) {
                id = dat.id_movimiento;
            }
            id++;
            printf("Numero de movimiento: %d \n", id);
            time_t now = time(NULL);
            struct tm *t = localtime(&now);
            o.fecha.mes=t->tm_mon+1;
            o.fecha.dia=t->tm_mday;
            o.fecha.year=t->tm_year+1900;
            o.id_movimiento = id;
            o.id_origen=reg.id;
            strcpy(o.tipo_transaccion,h);
            o.cuenta_destino=0;

            fwrite(&o, sizeof(Transferencia), 1, transfer_db);
            fclose(transfer_db);
        }
        else{
            fwrite(&reg, sizeof(Cuentascliente), 1, temporal);
        }
    }
    fclose(temporal);
    fclose(acount_db);
    if (bandera==1){
        remove(db_acount);
        rename("TempT.dat",db_acount);
        return;
    }
    printf("No se encontro en la base de datos, deposito anuado...\n");
    remove("TempT.dat");

}
void retiro(char db_path[256],char db_acount[256]){
    FILE *transfer_db;
    FILE *acount_db;
    FILE *temporal;
    transfer_db=fopen(db_path,"r+b");
    acount_db=fopen(db_acount,"rb");
    temporal=fopen("TempT.dat","w+b");
    Cuentascliente reg, p;
    Transferencia dat, o;
    int n=0,bandera=0;
    int saldo=0;
    printf("Ingrese Id_cuenta: ");
    scanf("%d", &n);
    while (fread(&reg, sizeof(Cuentascliente),1,acount_db)){
        if(reg.id==n){
            printf("Ingresa el monto a retirar: \n");
            scanf("%d", &o.temp_saldo);
            if(o.temp_saldo>=reg.saldo){
                break;
            }
            reg.saldo-=o.temp_saldo;
            fwrite(&reg, sizeof(Cuentascliente), 1, temporal);
            bandera++;
            int id =0;
            while (fread(&dat, sizeof(Transferencia), 1,transfer_db )) {
                id = dat.id_movimiento;
            }
            id++;
            char h[15]="Retiro";
            printf("Numero de movimiento: %d \n", id);

            time_t now = time(NULL);
            struct tm *t = localtime(&now);
            o.fecha.mes=t->tm_mon+1;
            o.fecha.dia=t->tm_mday;
            o.fecha.year=t->tm_year+1900;
            o.id_movimiento = id;
            o.id_origen=reg.id;
            strcpy(o.tipo_transaccion,h);
            o.cuenta_destino=0;


            fwrite(&o, sizeof(Transferencia), 1, transfer_db);
            fclose(transfer_db);
        }
        else{
            fwrite(&reg, sizeof(Cuentascliente), 1, temporal);
        }
    }
    fclose(temporal);
    fclose(acount_db);
    if (bandera==1){
        remove(db_acount);
        rename("TempT.dat",db_acount);
        return;
    }
    printf("Error a retirar o encontrar usuario. Retiro aunlado...\n");
    remove("TempT.dat");

}
void transferencia_dinero(char db_path[256],char db_acount[256]){
    FILE *transfer;
    FILE *acount_db;
    FILE *temporal;
    transfer=fopen(db_path,"r+b");
    acount_db=fopen(db_acount,"rb");
    temporal=fopen("TempTT.dat","w+b");
    Cuentascliente reg,p;
    Transferencia dat,o;
    int origen=0,destino=0;
    int saldo_origen,encuentra_origen=0,encuentra_destino=0, monto=0;
    printf("Ingresa id de origen: ");
    scanf("%d", &origen);
    while (fread(&reg, sizeof(Cuentascliente),1,acount_db)) {
        if (reg.id == origen) {
            encuentra_origen = 1;
            saldo_origen=reg.saldo;
            break;
        }
    }
    if (encuentra_origen==0){
        printf("La cuenta de origen no existe\n");
        fclose(transfer);
        fclose(temporal);
        fclose(acount_db);
        remove("TempTT.dat");
        return;
    }
    printf("Ingresa monto a transferir: ");
    scanf("%d",&monto);
    if(saldo_origen<monto){
        printf("No cuentas con el saldo suficiente\n");
        fclose(transfer);
        fclose(temporal);
        fclose(acount_db);
        remove("TempTT.dat");
        return;
    }
    rewind(acount_db);
    printf("Ingresa id de destino: ");
    scanf("%d", &destino);
    while (fread(&reg, sizeof(Cuentascliente),1,acount_db)) {
        if (reg.id == destino) {
            encuentra_destino = 1;
            break;
        }
    }
    if (encuentra_destino==0){
        printf("La cuenta de destino no existe\n");
        fclose(transfer);
        fclose(temporal);
        fclose(acount_db);
        remove("TempTT.dat");
        return;
    }
    rewind(acount_db);
    while (fread(&reg, sizeof(Cuentascliente),1,acount_db)) {
        if (reg.id == origen) {
            reg.saldo -= monto;
            fwrite(&reg, sizeof(Cuentascliente), 1, temporal);
        } else if (reg.id == destino) {
            reg.saldo += monto;
            fwrite(&reg, sizeof(Cuentascliente), 1, temporal);
        } else {
            fwrite(&reg, sizeof(Cuentascliente), 1, temporal);
        }
    }
    fclose(temporal);
    fclose(acount_db);
    remove(db_acount);
    rename("TempTT.dat",db_acount);
    int id=0;
    while (fread(&dat, sizeof(Transferencia),1,transfer)){
        id=dat.id_movimiento;
    }
    id++;
    printf("Id de movimiento: %d\n", id);

    char h[15]="Transferencia";

    time_t now = time(NULL);
    struct tm *t = localtime(&now);
    o.fecha.mes=t->tm_mon+1;
    o.fecha.dia=t->tm_mday;
    o.fecha.year=t->tm_year+1900;

    o.id_movimiento = id;
    o.id_origen=origen;
    o.cuenta_destino=destino;
    o.temp_saldo=monto;
    strcpy(o.tipo_transaccion,h);
    fwrite(&o, sizeof(Transferencia),1,transfer);
    fclose(transfer);

}
void registro(char db_path[256]){
    FILE *transfer_db;
    transfer_db=fopen(db_path,"rb");
    Transferencia reg;
        printf("Transaccion |Movimiento|Id origen |Id destino| Fecha(dd/mm/aaaa) | Monto\n");
    while(fread(&reg, sizeof(Transferencia), 1, transfer_db)){
        if(reg.cuenta_destino==0){
        printf("     %d     | %s      |   %d     |    NULL  | %d/%d/%d   |  %d \n", reg.id_movimiento,reg.tipo_transaccion,reg.id_origen,reg.fecha.dia,reg.fecha.mes,reg.fecha.year,reg.temp_saldo);

        }
        else{
        printf("     %d     | %s      |   %d     |    %d    | %d/%d/%d  |  %d \n", reg.id_movimiento,reg.tipo_transaccion,reg.id_origen,reg.cuenta_destino,reg.fecha.dia,reg.fecha.mes,reg.fecha.year,reg.temp_saldo);
        }
    }
    fclose(transfer_db);

}
void transfer(char db_path[256],char db_user[256]){
    int m=0;
    while (m!=5){
        printf("1: Debosito         :\n");
        printf("2: Retiro           :\n");
        printf("3: Transferencia    :\n");
        printf("4: Imprimir registro:\n");
        printf("5: Menu principal   :\n");
        scanf("%d", &m);
        switch (m){
            case 1:
                deposito(db_path,db_user);
                break;
            case 2:
                retiro(db_path,db_user);
                break;
            case 3:
                transferencia_dinero(db_path,db_user);
                break;
            case 4:
                registro(db_path);
                break;
            case 5:
                return;
            default:
                printf("Error...");
        }
    }
}
//----------------------------------Opcion movimientos

//----------------------------------Opcion cuentas
void cuenta_nueva(char db_path[256],char db_user[256]){
    FILE *account_db;
    FILE *customer_db;
    customer_db=fopen(db_user,"rb");
    Clientes cliente;
    int customer=-1;
    int find=0;
    printf("Ingresa id_Usuario: ");
    scanf("%d", &customer);
    while (fread(&cliente, sizeof(Clientes),1,customer_db)){
        if(cliente.id==customer){
            fclose(customer_db);
            find++;
        }
    }
    fclose(customer_db);
    if (!find){
        printf("El cliente no existe, no se puede crear cuenta\n");
    }
    else{
            account_db = fopen(db_path, "r+b");
            Cuentascliente p, reg;
            p.id_usuario=customer;
            printf("Saldo inicial: ");
            scanf("%d", &p.saldo);
            fflush(stdin);
            int id = 0;
            while (fread(&reg, sizeof(Cuentascliente), 1, account_db)) {
                id = reg.id;
            }
            id++;
            printf("Cuenta agregada con id: %d \n", id);
            p.id = id;
            time_t now = time(NULL);
            struct tm *t = localtime(&now);
            p.fecha.mes=t->tm_mon+1;
            p.fecha.dia=t->tm_mday;
            p.fecha.year=t->tm_year+1900;
            fwrite(&p, sizeof(Cuentascliente), 1, account_db);
            fclose(account_db);
    }
}
void cuenta_imprimir(char db_path[256], char db_user[256]){
    FILE *acount_db;
    acount_db=fopen(db_path,"rb");
    Cuentascliente reg;
               printf("Id_Cuenta | Nombre de usuario | Saldo | Fecha (dd/mm/aaaa)\n");
    while(fread(&reg, sizeof(Cuentascliente), 1, acount_db)){
        FILE *costumer_db;
        costumer_db=fopen(db_user,"rb");
        Clientes busqueda;
        while(fread(&busqueda, sizeof(Clientes),1,costumer_db)){
            if (busqueda.id==reg.id_usuario){
                printf("  %d     | %s %s %s |  %d  | %d/%d/%d ",reg.id,busqueda.nom,busqueda.app,busqueda.apm ,reg.saldo,reg.fecha.dia,reg.fecha.mes,reg.fecha.year);
                printf("\n");
            }
        }
        fclose(costumer_db);
    }
    fclose(acount_db);
}
void cuenta_buscar(char db_path[256], char db_user[256]){
    FILE *acount_db;
    acount_db=fopen(db_path,"rb");
    Cuentascliente reg;
    int busqueda=-1;
    printf("Ingresa id de cuenta: ");
    scanf("%d", &busqueda);
                  printf("Id_Cuenta |   Nombre cliente   | Saldo \n");
    while (fread(&reg, sizeof(Cuentascliente),1,acount_db)){
        if(reg.id==busqueda){
           FILE *customer_db;
           customer_db=fopen(db_user,"rb");
           Clientes busqueda2;
           while (fread(&busqueda2, sizeof(Clientes),1,customer_db)){
               if(busqueda2.id==reg.id_usuario){
                   printf("  %d   | %s %s %s   | %d", reg.id,busqueda2.nom,busqueda2.app,busqueda2.apm, reg.saldo);
                   printf("\n");
                   fclose(acount_db);
                   fclose(customer_db);
                   return;;
               }
           }
           fclose(acount_db);
           return;
        }
    }
    printf("No se encontro en la base de datos...\n");
    fclose(acount_db);
}
void cuenta_eliminar(char db_path[254]){
    FILE *acount_db;
    FILE *temporal;
    acount_db=fopen(db_path,"r+b");
    temporal=fopen("TempC.dat", "w+b");
    Cuentascliente reg;
    int elimina=0;
    int bandera=0;
    printf("Digite la Id_Cuenta que desea eliminar: ");
    scanf("%d", &elimina);
    while (fread(&reg, sizeof(Cuentascliente),1,acount_db)){
        if(reg.id==elimina){
            printf("Se elimino la cuenta con exito\n");
            bandera++;
        }
        else{
            fwrite(&reg, sizeof(Cuentascliente), 1, temporal);
        }
    }
    fclose(temporal);
    fclose(acount_db);
    if (bandera==1){
        remove(db_path);
        rename("TempC.dat",db_path);
        return;
    }
    printf("No se encontro en la base de datos...\n");
    remove("TempC.dat");

}
void account(char db_path[256],char db_user[256]){
    int m=0;
    while (m!=5){
        printf("1: Nueva centa   :\n");
        printf("2: Buscar  :\n");
        printf("3: Eliminar:\n");
        printf("4: Imprimir:\n");
        printf("5: Menu principal:\n");
        scanf("%d", &m);
        switch (m){
            case 1:
                cuenta_nueva(db_path,db_user);
                break;
            case 2:
                cuenta_buscar(db_path,db_user);
                break;
            case 3:
                cuenta_eliminar(db_path);
                break;
            case 4:
                cuenta_imprimir(db_path,db_user);
                break;
            case 5:
                return;
            default:
                printf("Error...");
        }
    }
}
//----------------------------------Opcion cuentas

//----------------------------------Opcion cliente
void cliente_nuevo(char db_path[256]) {

    FILE *customer_db;
    customer_db=fopen(db_path,"r+b");

    //Clientes *p=(Clientes *)malloc(sizeof(Clientes)*1);
    Clientes p,reg;

    printf("Nombre: ");
    scanf("%s", p.nom);
    fflush(stdin);
    printf("Apellido Paterno: ");
    scanf("%s", p.app);
    fflush(stdin);
    printf("Apellido Materno: ");
    scanf("%s", p.apm);
    fflush(stdin);
    printf("Fecha de nacimiento(dd/mm/aaaa)\n");
    printf("Dia: ");
    scanf("%d", &p.fecha.dia);
    fflush(stdin);
    printf("Mes: ");
    scanf("%d", &p.fecha.mes);
    fflush(stdin);
    printf("Año: ");
    scanf("%d", &p.fecha.year);

    int id=0;
    while(fread(&reg, sizeof(Clientes), 1, customer_db)){
        //printf("%s ", reg.nom);
        id=reg.id;
    }
    id++;
    printf("Cliente agregado con id: %d \n",id);
    p.id=id;
    fwrite(&p,sizeof(Clientes),1,customer_db);
    fclose(customer_db);
}
void cliente_imprimir(char db_path[256]){
    FILE *customer_db;
    customer_db=fopen(db_path,"rb");
    Clientes reg;
       printf("Id_Usuario | Nombre  Ap_Paterno  Ap_Materno | Fecha_Nacimiento\n");
    while(fread(&reg, sizeof(Clientes), 1, customer_db)){
        printf("    %d     | %s %s %s | %d/%d/%d    ", reg.id,reg.nom,reg.app,reg.apm,reg.fecha.dia,reg.fecha.mes,reg.fecha.year);
        printf("\n");
    }
    fclose(customer_db);
}
int cliente_buscar(char db_path[256], int imprimir){
    FILE *customer_db;
    customer_db=fopen(db_path,"rb");
    Clientes reg;
    int busqueda=-1;
    printf("Ingresa id_Usuario: ");
    scanf("%d", &busqueda);
    printf("Id_Usuario | Nombre  Ap_Paterno  Ap_Materno | Fecha_Nacimiento\n");
    while (fread(&reg, sizeof(Clientes),1,customer_db)){
        if(reg.id==busqueda){
            if(imprimir==1){
                printf("    %d     | %s %s %s | %d/%d/%d    ", reg.id,reg.nom,reg.app,reg.apm,reg.fecha.dia,reg.fecha.mes,reg.fecha.year);
                printf("\n");
            }
            fclose(customer_db);
            return 1;
        }
    }
    if(imprimir==1){
        printf("No se encontro en la base de datos...\n");
    }
    fclose(customer_db);
    return 0;
}
void cliente_eliminar(char db_path[256],char account_db[256]){
    Clientes reg;
    FILE *customer_db;
    FILE *customer_db_tmp;
    FILE *eliminacuenta;
    Cuentascliente revisa;
    eliminacuenta=fopen(account_db,"r+b");
    customer_db=fopen(db_path,"rb");
    customer_db_tmp=fopen("Temp.dat", "w+b");
    int eliminar=0, eliminado=0,bandera=0;
    printf("Ingresa Id_usuario: ");
    scanf("%d", &eliminar);
    while(fread(&reg, sizeof(Clientes), 1, customer_db)){
        if(reg.id==eliminar){
            printf("Eliminado con exito\n");
            eliminado++;
        }
        else{
            fwrite(&reg, sizeof(Clientes), 1, customer_db_tmp);
        }
    }
    fclose(customer_db);
    fclose(customer_db_tmp);
    if(eliminado==1){
        remove(db_path);
        rename("Temp.dat",db_path);
        customer_db_tmp=fopen("Temp.dat", "w+b");
        while(fread(&revisa, sizeof(Cuentascliente),1,eliminacuenta)){
            if(eliminar==revisa.id_usuario){
                printf("Cuenta %d eliminada con exito\n",revisa.id);
                bandera++;
            }

            fwrite(&revisa, sizeof(Cuentascliente),1,customer_db_tmp);
        }
        fclose(eliminacuenta);
        fclose(customer_db_tmp);
        if(bandera>0){
            remove(account_db);
            rename("Temp.dat",account_db);
        }
        else{
            printf("No se encontraron cuentas para eliminar\n");
            remove("Temp.dat");
        }
        return;
    }
    printf("No se encontro id en la base de datos para eliminar\n");
    remove("Temp.dat");
}
void customer(char db_path[256], char count_db[256]) {
    int m=0;
    printf("%s\n", db_path);
    while (m!=5){
        printf("1: Nuevo   :\n");
        printf("2: Buscar  :\n");
        printf("3: Eliminar:\n");
        printf("4: Imprimir:\n");
        printf("5: Menu principal:\n");
        scanf("%d", &m);
        switch (m){
            case 1:
                cliente_nuevo(db_path);
                break;
            case 2:
                cliente_buscar(db_path,1);
                break;
            case 3:
                cliente_eliminar(db_path,count_db);
                break;
            case 4:
                cliente_imprimir(db_path);
                break;
            case 5:
                return;
            default:
                printf("Error...");
        }
    }
}
//----------------------------------Opcion cliente

int main(void) {

    FILE *sys=NULL;
    FILE *customer_db=NULL;
    FILE *transfer_db=NULL;
    FILE *account_db=NULL;
    char *customer_file="\\Clientes.dat";
    char *account_file="\\Cuentas.dat";
    char *transfer_file="\\Transacciones.dat";
    char *db_path=(char*)malloc(sizeof(char));
    char tmp[244];

if ((sys=fopen("mydb.sys", "rb"))==0){
     printf("Ingresa la ruta donde quieres almacenar los archivos: ");
     scanf("%s", db_path);

     strcpy(tmp,db_path);
     customer_db=fopen(strcat(tmp,customer_file),"wb");
     if(customer_db==NULL){
         printf("No se pudo crear la base de datos para Clientes.\n");
         return 0;
     }

     strcpy(tmp,db_path);
     account_db=fopen(strcat(tmp,account_file),"wb");
    if(account_db==NULL){
        printf("No se pudo crear la base de datos para Cuentas.\n");
        return 0;
    }

     strcpy(tmp,db_path);
     transfer_db=fopen(strcat(tmp,transfer_file),"wb");
    if(transfer_db==NULL){
        printf("No se pudo crear la base de datos para Transferencias.\n");
        return 0;
    }
    sys=fopen("mydb.sys","a");
    fputs(db_path,sys);
    fclose(sys);
    fclose(customer_db);
    fclose(account_db);
    fclose(transfer_db);

    }

    sys = fopen("mydb.sys", "r");
    fscanf(sys, "%[^\n]", tmp);
    char path[256],path2[256], path3[256],path4[256], path5[256];
    printf("Bienvenido, la ruta de la base de datos es: %s\n", tmp);
    int m = 0;
    while (m != 4) {
        printf("<<Sistema MyDB>>\n");
        printf("1: Clientes\n");
        printf("2: Cuentas\n");
        printf("3: Transacciónes\n");
        printf("4: Salir\n");
        scanf("%d", &m);
        switch (m) {
            case 1:
                strcpy(path,tmp);
                strcpy(path5,tmp);
                customer(strcat(path,customer_file),strcat(path5,account_file));
                break;
            case 2:
                strcpy(path,tmp);
                strcpy(path2,tmp);
                account(strcat(path,account_file),strcat(path2,customer_file));
                break;
            case 3:
                strcpy(path3,tmp);
                strcpy(path4,tmp);
                transfer(strcat(path3,transfer_file),strcat(path4,account_file));
                break;
            case 4:
                break;

            default:
                break;
            }
        }

    return 0;
}