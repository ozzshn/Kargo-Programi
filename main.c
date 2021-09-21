#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <limits.h>
#include <windows.h>

#define INF 99999999

int * yollar;
int * mesafeler;
void yolbul();
void yolyaz();
void haritadagoster();
char yolbirlestir[400];

FILE *sehirKodlari,*sehirMesafeler;
char buff[255];
int toplamSehir = 0;

char **sehirKodlariDizisi;
char  **sehirisimleri;

int sehirKoduBul(char *sehirAdi)
{

    for(int i=1; i<toplamSehir+1; i++)
    {
        if(!strcmp(sehirKodlariDizisi[i],sehirAdi))
        {
            return i;
        }

    }
    return 0;
}

int dijsktra(int maliyetDizisi[toplamSehir+1][toplamSehir+1],int merkezSehir,int hedefSehir)
{
    int mesafe[toplamSehir+1],onceki[toplamSehir+1];
    int gidilmis[toplamSehir+1];
    int baslangic;
    char yol[toplamSehir+1];

    for(int i=0; i< toplamSehir+1; i++)
    {
        mesafe[i] = INF;  /// Tum mesafeler sonsuz
        onceki[i] = -1;   /// Onceki mesafeler -1 (gidilmemiþ)
        gidilmis[i] = 0; /// Gidilmiþ þehirler 0
    }

    baslangic = merkezSehir; /// Baslangic olarak merkez sehiri ayarla
    gidilmis[baslangic]=1;  /// Baslangic sehrine git
    mesafe[baslangic] = 0;  /// Baslangica gidis mesafesi 0
    int min, m, d,s = 0;
    while(gidilmis[hedefSehir] == 0 && s<=toplamSehir) /// Hedef sehire gidilene kadar devam et
    {
        min = INF; /// Min uzaklik sonsuz
        m = 0;
        for(int i=1; i< toplamSehir+1; i++) /// Matris 0 da sehir bilgisi yok o yuzden 1 den basla
        {
            d = mesafe[baslangic] + maliyetDizisi[baslangic][i]; /// Mesafeyi bul suan ki deger ve yeni yol
            if(d < mesafe[i] && gidilmis[i]==0)  /// Daha kisa ve gidilmemis ise onu sec
            {
                mesafe[i] = d; /// Yeni kisa yolu yaz
                onceki[i] = baslangic; /// Onceki mesafeyi tut
            }
            if(min>mesafe[i] && gidilmis[i]==0)  /// Min mesafe den kisa ve gidilmemis ise yeni min o
            {
                min = mesafe[i];
                m = i;
            }
        }
        baslangic = m; /// Yeni dugumu baslangica ata hedefe gidene kadar dongu devam
        gidilmis[baslangic] = 1; /// Gidildi yap
        s++;
    }

    baslangic = hedefSehir; /// Hedef sehirin kodunu baslangica ata
    int j = 0;

    while(baslangic != -1)   /// Hedef sehire gidene kadar olan yolu tut
    {
        yol[j++] = baslangic + 65; /// strrev kullanabilmek icin chara donüstür 65 ekleyerek
        baslangic = onceki[baslangic];
    }

    yol[j]='\0';
    strrev(yol); /// Terse çevir
    return mesafe[hedefSehir]; /// En kisa yolu dondur
}

int sehirKodlariDosyasiniAc()
{

    sehirKodlari = fopen("sehirkod.txt","r"); /// Dosyayi ac
    if(sehirKodlari == NULL) /// Acildi mi kontrol et
    {
        printf("Sehir kodlari dosyasi bulunamadi!\n");
        return -1;
    }

    char * kod;
    char * sehiradi;

    while( (fscanf(sehirKodlari, "%s", buff)) != EOF) /// Dosya sonuna gelene kadar oku
    {

        sehiradi = strtok(buff,"-"); /// Dosyadan okunan satiri buffer da tutmustuk onu '-' karakterine kadar ayiriyoruz
        kod = strtok(NULL,"-");  ///    Kaldigi yerden kalan kismi al
    }

    toplamSehir = atoi(kod); /// Son sehirin kodu toplam sehir sayisina esitle

    rewind(sehirKodlari); /// Dosyanin basina git

    sehirKodlariDizisi = (char **) malloc((toplamSehir+1) * sizeof(char *) ); /// Toplam sehir miktarý kadar matris olustur
    for(int j = 0; j<toplamSehir+1; j++) /// Matrisin sutunlari icin yer al
        sehirKodlariDizisi[j] = (char*)malloc(20*sizeof(char));

    while( (fscanf(sehirKodlari, "%s", buff)) != EOF) /// Dosya sonuna kadar oku
    {

        sehiradi = strtok(buff,"-");
        kod = strtok(NULL,"-");
        int i = atoi(kod);
        /// Parcalama islemi sonrasi sehir kodunun oldugu index e sehir adini yaz
        strcpy(sehirKodlariDizisi[i],sehiradi);

        printf("%s - %s\n",kod,sehiradi);
        fflush(stdin);
    }

    fclose(sehirKodlari); /// dosyayi kapat
    return 1; /// islem basarili
}

int mesafelerDosyasiniAc(int komsulukDizisi[toplamSehir+1][toplamSehir+1])
{

    sehirMesafeler = fopen("sehirmesafe.txt","r"); /// Sehir mesafe txt dosyasini bul ve ac

    if(sehirMesafeler == NULL) /// Acilamadi ise
    {
        printf("Sehir Mesafe dosyasi acilamadi!");
        return -1;
    }

    /// Dosya formati Sehir1-Sehir2-Mesafe
    char * sehir1;
    char * sehir2;
    char * mesafe;


    for(int i = 0; i<toplamSehir+1; i++)
    {
        for(int j = 0; j<toplamSehir+1; j++)
        {
            komsulukDizisi[i][j] = INF; ///Komsuluk matrisini olustur.Ýlk olarak hepsine sonsuz ata
        }
    }


    while((fscanf(sehirMesafeler,"%s",buff)) != EOF) /// Dosya sonuna kadar satir satir oku
    {
        /// Her satiri parcalara ayir
        sehir1 = strtok(buff,"-");
        sehir2 = strtok(NULL,"-");
        mesafe = strtok(NULL,"-");

        int x = sehirKoduBul(sehir1); /// Sehir adi dosyadan okunduktan sonra onun kodunu bul x e ata
        int y = sehirKoduBul(sehir2); /// Sehir adi dosyadan okunduktan sonra onun kodunu bul y e ata
        int w = atoi(mesafe); /// Aralarinda ki mesafeyi string ten int e cevir

        komsulukDizisi[x][y] = komsulukDizisi[y][x] = w; /// Iki sehrin kodlari matrisin indexleri olur degeri aralarinda ki mesafedir
        /// matris[1][2] demek 1 kodlu sehirden 2 kodlu sehire gidilecek mesafedir
        fflush(stdin);
    }

    fclose(sehirMesafeler); /// Dosyayi kapat
    return 1; /// islem basarili
}



int main()
{

    if(sehirKodlariDosyasiniAc() == -1) /// Dosya acilmazsa sonlandir
    {
        return -1;
    }

    int komsulukMatrisi[toplamSehir+1][toplamSehir+1]; /// Sehir sayisini ogrendikten sonra sehirsayisinin bir fazlasi boyutunda matris olustur

    if(mesafelerDosyasiniAc(komsulukMatrisi) == -1) /// Mesafe dosyasi acilmazsa sonlandir
    {
        return -1;
    }


PBAS:
    printf("\n\n------------ GEZGIN KARGOYA HOSGELDINIZ-------------- \n");

    int teslimat_sayisi;

sayial:
    printf("Teslimat yapilacak sehir sayisini giriniz:");///Teslimat adetini al
    scanf("%d",&teslimat_sayisi);

    if(teslimat_sayisi<=1 || teslimat_sayisi>=11)///Teslimat adedini maksimum 10'la sinirla
    {
        printf("Hatali sayi girdiniz!\n\n");
        goto sayial;
    }


    int merkezSehirKodu,hedefSehirKodu[50];

    hedefSehirKodu[0]=41;///Baslangic elemanina Kocaeli ata
    hedefSehirKodu[teslimat_sayisi+1]=41;///Bitis elemanina Kocaeli ata


    printf("\t\nBaslangic sehrinin kodu %d dir.\n",hedefSehirKodu[0] );

    for(int i=1; i<teslimat_sayisi+1; i++)
    {
        hedefSehirKodu[0]=41;
        hedefSehirKodu[teslimat_sayisi+1]=41;
        printf("  %d. teslimat sehrinin kodunu giriniz:",i);///Teslimat yapilacak sehirlerin kodlarini al
        scanf("%d",&hedefSehirKodu[i]);

        if(!(hedefSehirKodu[i]>=1 && hedefSehirKodu[i]<= toplamSehir))///Sehir kodlarini 1-81 araliginda tut
        {
            printf("Girdiginiz kodda sehir bulunmamaktadir. (1-81) araliginda kod giriniz.\n");
        }
    }
    printf("Bitis sehrinin kodu %d dir.\n\n\n",hedefSehirKodu[teslimat_sayisi+1] );

    int toplamYol=0;

    int uzaklik[100];
    for(int i=0; i<teslimat_sayisi+1; i++)
    {
        uzaklik[i]=dijsktra(komsulukMatrisi,hedefSehirKodu[i],hedefSehirKodu[i+1]);
        printf("%d -> %d kodlu sehir arasi en kisa mesafe %d\n",hedefSehirKodu[i],hedefSehirKodu[i+1],uzaklik[i]);///En kisa mesafeyi buldur
        toplamYol+=uzaklik[i];

    }

    printf("\ttoplam gidilen mesafe: %d km \n\t",toplamYol);///Toplam yolu yazdir

}
   /*

    ///Farkli mesafeleri sirala

    int yeni_hedef_kodlari[10];
    int adet,temp;
    int enkisa[10];
    adet=teslimat_sayisi+1;
    int k,m,i,j;
    int enk;
    int yer=1;
    for( i=0; i<1; i++)
    {
        for(j=i+1; j<teslimat_sayisi+1; j++)
        {

            if(i!=j)
            {
                uzaklik[j]=dijsktra(komsulukMatrisi,hedefSehirKodu[i],hedefSehirKodu[j]);
                printf("%d'den %d'ye olan uzaklik: %d\n",hedefSehirKodu[i],hedefSehirKodu[j],uzaklik[j]);
            }
            for(k=0; k<adet-1; k++)
            {
                for(m=k+1; j<adet; j++)
                {
                    if(uzaklik[k]>uzaklik[m])
                    {
                        temp=uzaklik[k];
                        uzaklik[k]=uzaklik[m];
                        uzaklik[m]=temp;

                    }
                }
            }

        }




*/
