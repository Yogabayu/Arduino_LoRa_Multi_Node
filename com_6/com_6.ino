// **GATEWAY / MASTER**

#include <LoRa.h>
#include <ArduinoJson.h>

#define SS 53
#define RST 43
#define DIO0 45

String Sendernode;

void setup()
{
  Serial.begin(9600);
  while (!Serial)
    ;
  Serial.println("Receiver Host");
  LoRa.setPins(SS, RST, DIO0);
  if (!LoRa.begin(433E6))
  {
    Serial.println("Starting LoRa Failed!");
    while (1)
      ;
  }
}
void loop()
{
  int packetSize = LoRa.parsePacket(); // cek apakah ada paket baru yang diterima
  if (packetSize)                      // jika true
  {
    int sender = LoRa.read(); // cek header dari paket yang diterima

    String incoming = ""; //variabel untk menampung data
    Serial.print("Receiving Packet: ");
    while (LoRa.available()) // ketika LoRa siap terima paket
    {
      incoming += (char)LoRa.read(); // memasukkan paket yang dibaca ke variabel incoming
      //      Serial.println((char)LoRa.read());
    }
    if (sender == 1) //jika header bernilai 1
    {
      Sendernode = "node1";
      Serial.print("Pengirim: " + Sendernode);
      Serial.println(" - Landing");

      DynamicJsonDocument doc(200);   //menggunakan DynamicJson karena paket bisa berukuran lebih dari 1KB, jika lebih kecil dari 1KB pakai StaticJson
      deserializeJson(doc, incoming); //parsing data yang diterima ke dalam variabel doc

      //bentuk data yang diterima dalam variabel doc :
      // {"suhu":xx.xx , "kelem":xx.xx , "hari":xx , "bulan":xx , "tahun":xxxx , "kec":xx.xx , "arah":"arahAngin" , "jam":xx , "menit":xx}

      //memasukkan data parsing ke variabel
      float suhu = doc["suhu"]; // cara pengambilan data parsing doc["namaParameterParsing"], dan harus sama antara pengirim dan penerima
      float kelem = doc["kelem"];
      int hari = doc["hari"];
      int bulan = doc["bulan"];
      int tahun = doc["tahun"];
      float kec = doc["kec"];
      String arah = doc["arah"];
      int jam = doc["jam"];
      int menit = doc["menit"];

      //menampilkan data di serial monitor
      Serial.print(hari);
      Serial.print(" / ");
      Serial.print(bulan);
      Serial.print(" / ");
      Serial.print(tahun);
      Serial.print(" ---- ");
      Serial.print(jam);
      Serial.print(":");
      Serial.println(menit);
      Serial.print("Temperature: ");
      Serial.print(suhu);
      Serial.print(" °C ");
      Serial.print(", Humidity: ");
      Serial.print(kelem);
      Serial.println(" % ");
      Serial.print("Kecepatan angin: ");
      Serial.print(kec);
      Serial.print(" [m/s] ");
      Serial.print("Arah angin: ");
      Serial.println(arah);
      Serial.println("==================");
    }
    else //jika header bernilai selain 1
    {
      Sendernode = "node2";
      Serial.print("Pengirim: " + Sendernode);
      Serial.println(" - Landing");

      DynamicJsonDocument doc(200);   //menggunakan DynamicJson karena paket bisa berukuran lebih dari 1KB, jika lebih kecil dari 1KB pakai StaticJson
      deserializeJson(doc, incoming); //parsing data yang diterima ke dalam variabel doc

      //bentuk data yang diterima dalam variabel doc :
      // {"suhu":xx.xx , "kelem":xx.xx , "hari":xx , "bulan":xx , "tahun":xxxx , "kec":xx.xx , "arah":"arahAngin" , "jam":xx , "menit":xx}

      //memasukkan data parsing ke variabel
      float suhu = doc["suhu"]; // cara pengambilan data parsing doc["namaParameterParsing"], dan harus sama antara pengirim dan penerima
      float kelem = doc["kelem"];
      int hari = doc["hari"];
      int bulan = doc["bulan"];
      int tahun = doc["tahun"];
      float kec = doc["kec"];
      String arah = doc["arah"];
      int jam = doc["jam"];
      int menit = doc["menit"];

      //menampilkan data di serial monitor
      Serial.print(hari);
      Serial.print(" / ");
      Serial.print(bulan);
      Serial.print(" / ");
      Serial.print(tahun);
      Serial.print(" ---- ");
      Serial.print(jam);
      Serial.print(":");
      Serial.println(menit);
      Serial.print("Temperature: ");
      Serial.print(suhu);
      Serial.print(" °C ");
      Serial.print(", Humidity: ");
      Serial.print(kelem);
      Serial.println(" % ");
      Serial.print("Kecepatan angin: ");
      Serial.print(kec);
      Serial.print(" [m/s] ");
      Serial.print("Arah angin: ");
      Serial.println(arah);
      Serial.println("==================");
    }
  }
  delay(50);
}
