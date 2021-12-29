#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include "HX711.h"
//#include "FloatToString.h"

#define calibration_factor -9720.00




#define zero_factor -758080








 


#define DOUT 4 // D2 maps to GPIO5
#define CLK 5  // D1 maps to GPIO4



int MAX_ATTEMPTS = 5; // maximo numero de tentativas de conexão com o AP antes de desistir e ir dormir
int wps_connect_button = 12;  // botão de wps no pino d7
int reset_cache_config = 13; //botão de apagar cache. (para apagar cache, manter pressionado durante energização ou reset)
String response = ""; //variável resposta da api. Usada somente para fins de depuração.
bool canIPost = false; //estado que indica que conexão com AP foi feita e pode-se fazer o post.


//-----------------------------------------------------------funções------------------------------------------------- : rotinas usadas
// @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@ função print_info(): imprime informações do status de conexão, senha, ssid, ip, estado da conexão (lista de possiveis estados comentados no fim do codigo)
void print_info(){
      Serial.printf("ssid: %s\n", WiFi.SSID().c_str());
      Serial.printf("psk: %s\n", WiFi.psk().c_str());
      Serial.printf("ip: %s\n",  WiFi.localIP().toString().c_str());
      Serial.printf("estado da conexão: %d\n", WiFi.status());
  }

// @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@ função postToServer: tenta fazer post uma vez que conexão com o AP tenha sido feita.
void postToServer(String code, float liquid){

   String json = 

"{\"code\":\""+code+"\","+
"\"liquid\":\""+liquid+"\"}";
  

   HTTPClient http;

  
  if(http.begin("http://nivelgas.com.br/api/measurements")){
 
  http.addHeader("accept", "application/json");
  http.addHeader("content-type", "application/json");
  int http_code = http.POST(json);
 
  if(http_code >= 200){
     Serial.printf("codigo de retorno http: %d \n\n", http_code);
     response = http.getString();
     Serial.printf("resposta http (json): %s \n", response.c_str());
    
    }else{
      Serial.printf("falha na requisicao http: %d \n", http_code);
      }

  }else{
    Serial.printf("falha no begin");
    }
}

// @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@ função connectOrReconnect: reconecta a ap previamente já conectado ou armazena senha e conecta caso o ap seja novo.
void connectOrReconnect(){
  canIPost = false; // sem conexão, nem tenta postar
  int try_ = 0; // numero atual de tentativas de conectar com o AP
  while((WiFi.status() != WL_CONNECTED)  &&  (try_<= MAX_ATTEMPTS)){ // Enquanto estiver disconectado e o numero maximo de tentativas não tiver sido feito continua em loop
   
  ESP.wdtFeed(); // alimenta watch dog pra não travar
  if(!(WiFi.psk().length() > 0)){ // se tamanho da senha armazenada não for maior que zero (sem senha armazenada), inicia processo de obtenção da senha pelo wps
    // ps: esse metodo de teste não é muito elegante... vou tentar usar outro no futuro
    Serial.println("apertar botao wps \n");

             /*while(true){ // loop para obter senha por wps
                    ESP.wdtFeed(); //alimenta watch dog pra não travar
                    if(digitalRead(wps_connect_button) == HIGH){  //espera em loop até botão wps ser pressionado (pressionar primeiro o do roteador)
                     WiFi.beginWPSConfig(); // tenta negociação de senha e conexao pelo wps
                     delay(5000); // espera necessária... se não não rola
                     print_info();// se tiver dado certo printa já as informações da conexão
                               if(WiFi.status() == WL_CONNECTED){ // se conectou
                                canIPost = true; // dá autorização para fazer o post
                                Serial.println("\nOperacao wps bem sucedida\n");
                                break; // sai do laço de obtenção de senha por wps (nesse ponto a conexão já foi feita e a senha armazenada na flash)
                                }else{ //senao continue tentando
                                  Serial.println("\nOperacao wps mal sucedida... continue tentando\n");
                                  }
                    }
             }*/
             const char* ssid = "ssid_name";
             const char* password = "ssid_password";
             WiFi.begin(ssid, password);
             if(WiFi.status() == WL_CONNECTED){ // se conectou
                                canIPost = true; // dá autorização para fazer o post
                                Serial.println("\nOperacao wps bem sucedida\n");
                                break; // sai do laço de obtenção de senha por wps (nesse ponto a conexão já foi feita e a senha armazenada na flash)
                                }else{ //senao continue tentando
                                  Serial.println("\nOperacao wps mal sucedida... continue tentando\n");
                                  }
    }
    else{ // caso já tenha senha armazenada só conecta
      Serial.println("ja tem configuracao armazenada \n");
      //WiFi.begin(); // conecta com senha e ssid previamente armazenados
      const char* ssid = "ssid_name";
      const char* password = "ssid_password";
      WiFi.begin(ssid, password);
      delay(4000);// delay necessário
      print_info();
                   if(WiFi.status() == WL_CONNECTED){ // se conectou
                  canIPost = true; // dá autorização pra fazer o post
                  }
      }
try_++; // caso já tenha senha armazenada, tem direito a cinco tentativas de conectar com o AP.
}
  }
//-----------------------------------------------------------funções fim-------------------------------------------------
void setup() {
  ESP.wdtDisable();
  ESP.wdtEnable(WDTO_8S);
  
  pinMode(wps_connect_button, INPUT);
  pinMode(reset_cache_config, INPUT);
  Serial.begin(9600);
  if(digitalRead(reset_cache_config) == HIGH){  // se botao de limpar cache está apertado durante a inicialização ou reset, apaga cache
      Serial.println("\n*resetando cache*\n");
      WiFi.disconnect(); // apaga senha pré gravada
    }
// +++++++++++++++zona de coisas referentes a balaça+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

// danilo, usar essa area para colocar o código referente a obteção do peso. Nessa area obter peso e transformar em string para passar como parametro no segundo parametro
// da função postToServer abaixo:
    ESP.wdtFeed();
    HX711 scale(DOUT, CLK);
    scale.set_scale(calibration_factor);
    scale.set_offset(zero_factor);
    ESP.wdtFeed();
    float data = scale.get_units();
    data = scale.get_units();
    data = scale.get_units();
    Serial.println("Kg");
    float kg = (data*(0.453592));
    Serial.println(kg, 2);
    ESP.wdtFeed();
    
    char charVal[5];               //temporarily holds data from vals 
    String stringVal = "";     //data on buff is copied to this string
    dtostrf(kg, 5, 2, charVal);  //4 is mininum width, 4 is precision; float value is copied onto buff
    //display character array
    Serial.print("charVal: ");
    for(int i=0;i<sizeof(charVal);i++)
    {
      ESP.wdtFeed();
    Serial.print(charVal[i]);
    }
    Serial.println();
   //convert chararray to string
    for(int i=0;i<sizeof(charVal);i++)
    {
     ESP.wdtFeed();
     stringVal+=charVal[i];
    }
    Serial.print("stringVal: ");Serial.println(stringVal); //display string
    
   // stringVal="25.00";
   // Serial.print("stringVal: ");Serial.println(stringVal); //display string
    ESP.wdtFeed();
    

// +++++++++++++++ fim da zona de coisas referentes a balaça+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
  ESP.wdtFeed();
  delay(1000); // delay necessário
  ESP.wdtFeed();
  WiFi.mode(WIFI_STA); // esp no modo station -> conecta a outro ap
  delay(1000);// delay necessário
  ESP.wdtFeed();
  Serial.println("\n INICIANDO OU ACORDANDO --------------- \n");
  connectOrReconnect(); // chama função acima descrita...
            if(canIPost){  // se conectou, então tem autorização para fazer o post
             ESP.wdtFeed();
             postToServer("3Jh6jhjtL9", kg/*colocque o valor transformado em string aqui*/); // <= peso transformado em string aqui!
             ESP.wdtFeed();
             Serial.println("\n tentativa de post realizada indo dormir... \n");
             ESP.wdtFeed();
             delay(1000); // delay necessário
             ESP.wdtFeed();
             ESP.deepSleep(300000000); // 40*60*1000000 = 2400000000 => 40 minutos // põe pra dormir em deepsleep mode
            }else{// se tentou conectar mais de 5 vezes com o AP e fracassou, desista e durma
              Serial.println("\n tentativa de conexao fracassada indo dormir... \n");
              ESP.wdtFeed();
              delay(1000);// delay necessário
              ESP.wdtFeed();
              ESP.deepSleep(300000000); // 40*60*1000000 = 2400000000 => 40 minutos
              }
}

void loop() {
ESP.wdtFeed();

}


//    WL_NO_SHIELD        = 255,   
//    WL_IDLE_STATUS      = 0,
//    WL_NO_SSID_AVAIL    = 1,
//    WL_SCAN_COMPLETED   = 2,
//    WL_CONNECTED        = 3, -> conectado
//    WL_CONNECT_FAILED   = 4,
//    WL_CONNECTION_LOST  = 5,
//    WL_DISCONNECTED     = 6 


//      HTTP_CODE_CONTINUE = 100,
//    HTTP_CODE_SWITCHING_PROTOCOLS = 101,
//    HTTP_CODE_PROCESSING = 102,
//    HTTP_CODE_OK = 200,
//    HTTP_CODE_CREATED = 201,
//    HTTP_CODE_ACCEPTED = 202,
//    HTTP_CODE_NON_AUTHORITATIVE_INFORMATION = 203,
//    HTTP_CODE_NO_CONTENT = 204,
//    HTTP_CODE_RESET_CONTENT = 205,
//    HTTP_CODE_PARTIAL_CONTENT = 206,
//    HTTP_CODE_MULTI_STATUS = 207,
//    HTTP_CODE_ALREADY_REPORTED = 208,
//    HTTP_CODE_IM_USED = 226,
//    HTTP_CODE_MULTIPLE_CHOICES = 300,
//    HTTP_CODE_MOVED_PERMANENTLY = 301,
//    HTTP_CODE_FOUND = 302,
//    HTTP_CODE_SEE_OTHER = 303,
//    HTTP_CODE_NOT_MODIFIED = 304,
//    HTTP_CODE_USE_PROXY = 305,
//    HTTP_CODE_TEMPORARY_REDIRECT = 307,
//    HTTP_CODE_PERMANENT_REDIRECT = 308,
//    HTTP_CODE_BAD_REQUEST = 400,
//    HTTP_CODE_UNAUTHORIZED = 401,
//    HTTP_CODE_PAYMENT_REQUIRED = 402,
//    HTTP_CODE_FORBIDDEN = 403,
//    HTTP_CODE_NOT_FOUND = 404,
//    HTTP_CODE_METHOD_NOT_ALLOWED = 405,
//    HTTP_CODE_NOT_ACCEPTABLE = 406,
//    HTTP_CODE_PROXY_AUTHENTICATION_REQUIRED = 407,
//    HTTP_CODE_REQUEST_TIMEOUT = 408,
//    HTTP_CODE_CONFLICT = 409,
//    HTTP_CODE_GONE = 410,
//    HTTP_CODE_LENGTH_REQUIRED = 411,
//    HTTP_CODE_PRECONDITION_FAILED = 412,
//    HTTP_CODE_PAYLOAD_TOO_LARGE = 413,
//    HTTP_CODE_URI_TOO_LONG = 414,
//    HTTP_CODE_UNSUPPORTED_MEDIA_TYPE = 415,
//    HTTP_CODE_RANGE_NOT_SATISFIABLE = 416,
//    HTTP_CODE_EXPECTATION_FAILED = 417,
//    HTTP_CODE_MISDIRECTED_REQUEST = 421,
//    HTTP_CODE_UNPROCESSABLE_ENTITY = 422,
//    HTTP_CODE_LOCKED = 423,
//    HTTP_CODE_FAILED_DEPENDENCY = 424,
//    HTTP_CODE_UPGRADE_REQUIRED = 426,
//    HTTP_CODE_PRECONDITION_REQUIRED = 428,
//    HTTP_CODE_TOO_MANY_REQUESTS = 429,
//    HTTP_CODE_REQUEST_HEADER_FIELDS_TOO_LARGE = 431,
//    HTTP_CODE_INTERNAL_SERVER_ERROR = 500,
//    HTTP_CODE_NOT_IMPLEMENTED = 501,
//    HTTP_CODE_BAD_GATEWAY = 502,
//    HTTP_CODE_SERVICE_UNAVAILABLE = 503,
//    HTTP_CODE_GATEWAY_TIMEOUT = 504,
//    HTTP_CODE_HTTP_VERSION_NOT_SUPPORTED = 505,
//    HTTP_CODE_VARIANT_ALSO_NEGOTIATES = 506,
//    HTTP_CODE_INSUFFICIENT_STORAGE = 507,
//    HTTP_CODE_LOOP_DETECTED = 508,
//    HTTP_CODE_NOT_EXTENDED = 510,
//HTTP_CODE_NETWORK_AUTHENTICATION_REQUIRED = 511
