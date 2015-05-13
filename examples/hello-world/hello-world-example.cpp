/*
 * Copyright (c) 2015 Samsung Electronics Co., Ltd.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 */

#include <stdio.h>
#include <math.h>

#include <dali/dali.h>
#include <dali-toolkit/dali-toolkit.h>

using namespace Dali;
using namespace Toolkit;

#define TOUCH_ENABLED 0

const std::string LIGHT_DIRECTION_PROPERTY_NAME("uLightDirection");
const std::string TIMEZONE_COLOR_UPPER_PROPERTY_NAME("uTimezoneColorUpper");
const std::string TIMEZONE_COLOR_LOWER_PROPERTY_NAME("uTimezoneColorLower");

const char * const WORLDMAP_IMAGE(DALI_IMAGE_DIR "worldmap.png");

float x_coordinate_billboard[] =
{
  178.4034    ,  // Midway
  -161.05642  ,  // Honolulu
  -158.6153   ,  // Hawaii
  -155.05981  ,  // Alaska
  -150.01314  ,  // Anchorage
  -122.67556  ,  // San Francisco
  -123.69496  ,  // Vancouver
  -121.821335 ,  // Seattle
  -119.74172  ,  // Los Angeles
  -116.93543  ,  // Tijuana
  -113.732445 ,  // Calgary
  -112.09933  ,  // Phoenix
  -107.62116  ,  // Mazatlan
  -104.53391  ,  // Denver
  -99.5546    ,  // Mexico City
  -97.758876  ,  // Winnipeg
  72.86989    ,  // Diego Garcia
  -98.853325  ,  // Dallas
  -88.41896   ,  // Chicago
  -79.294655  ,  // Toronto
  -81.35469   ,  // Miami
  -77.63689   ,  // Lima
  -79.54106   ,  // Quito
  -78.48526   ,  // Washington DC
  -75.53131   ,  // Ottawa
  -74.59105   ,  // New York
  -73.38983   ,  // Bogota
  -71.60972   ,  // Santiago
  -68.0685    ,  // La Paz
  -66.77664   ,  // Caracas
  -58.828273  ,  // Buenos Aires
  -58.12603   ,  // Newfoundland
  -62.24370   ,  // St.John\'s
  -47.022537  ,  // Sao Paulo
  -47.544933  ,  // Brasilia
  -42.50564   ,  // Rio de Janeiro
  -32.306248  ,  // Fernando de Noronha
  -29.218638  ,  // Azores
  -9.441385   ,  // Lisbon
  -7.273542   ,  // Casablanca
  -6.93398    ,  // Dublin
  -6.8623484  ,  // Rabat
  -3.8725073  ,  // Madrid
  -64.61495   ,  // The Settlement
  -0.354684   ,  // London
  -77.08105   ,  // Kingston
  1.835886    ,  // Barcelona
  2.2791698   ,  // Paris
  3.2899761   ,  // Algiers
  10.397419   ,  // Tunis
  16.067972   ,  // Tripoli
  13.407970   ,  // Berlin
  12.641033   ,  // Rome
  15.032436   ,  // Prague
  13.547617   ,  // Kinshasa
  18.05532    ,  // Stockholm
  17.756683   ,  // Bratislava
  18.393995   ,  // Cape Town
  20.545227   ,  // Budapest
  21.05297    ,  // Warsaw
  20.494545   ,  // Kaliningrad
  24.89072    ,  // Sofia
  23.428997   ,  // Athens
  25.257885   ,  // Lviv
  23.356843   ,  // Helsinki
  23.719614   ,  // Riga
  23.795825   ,  // Tallinn
  27.231106   ,  // Bucharest
  25.019764   ,  // Vilnius
  26.650738   ,  // Lubumbashi
  27.393673   ,  // Minsk
  29.74388    ,  // St. Petersburg
  27.800713   ,  // Istanbul
  160.6611    ,  // Kamchatka
  31.134285   ,  // Kiev
  34.71484    ,  // Jerusalem
  37.18888    ,  // Moscow
  35.346825   ,  // Kharkiv
  36.92116    ,  // Nairobi
  39.35996    ,  // Jeddah
  44.600845   ,  // Volgograd
  44.25055    ,  // Yerevan
  44.802197   ,  // Tbilisi
  48.857042   ,  // Baku
  115.62239   ,  // Chita
  49.841843   ,  // Samara
  52.205284   ,  // Tehran
  52.150777   ,  // Izhevsk
  54.824905   ,  // Abu Dhabi
  55.641846   ,  // Dubai
  56.679527   ,  // Ufa
  62.52423    ,  // Yekaterinburg
  62.784447   ,  // Chelyabinsk
  67.243535   ,  // Karachi
  68.0086     ,  // Kabul
  69.31265    ,  // Tashkent
  72.875125   ,  // Islamabad
  72.61432    ,  // Mumbai
  72.74438    ,  // Omsk
  77.56143    ,  // Almaty
  76.77073    ,  // Bangalore
  78.75937    ,  // Hyderabad
  77.00605    ,  // Delhi
  79.53555    ,  // Colombo
  79.61336    ,  // Chennai
  83.68338    ,  // Novosibirsk
  85.05195    ,  // Kathmandu
  88.66081    ,  // Novokuznetsk
  88.24824    ,  // Kolkata
  90.34825    ,  // Dhaka
  91.3947     ,  // Hovd
  92.762544   ,  // Krasnoyarsk
  100.20229   ,  // Bangkok
  101.54424   ,  // Kuala Lumpur
  103.5532    ,  // Singapore
  104.10452   ,  // Irkutsk
  105.04873   ,  // Phnom Penh
  105.84205   ,  // Hanoi
  106.88372   ,  // Jakarta
  105.59384   ,  // Ulan Bator
  114.18268   ,  // Hong Kong
  115.7593    ,  // Perth
  116.392316  ,  // Beijing
  120.96371   ,  // Manila
  121.48876   ,  // Taipei
  127.21141   ,  // Seoul
  128.12206   ,  // Ambon
  132.32596   ,  // Vladivostok
  129.36038   ,  // Yakutsk
  134.32286   ,  // Khabarovsk
  138.21642   ,  // Adelaide
  139.46527   ,  // Tokyo
  142.6483    ,  // Yuzhno-Sakhalinsk
  -63.36105   ,  // Halifax
  144.71287   ,  // Guam
  148.78387   ,  // Canberra
  152.60901   ,  // Magadan
  150.84053   ,  // Sydney
  158.2654    ,  // Petropavlovsk
  172.6642    ,  // Tarawa
  174.302     ,  // Auckland
  174.72213   ,  // Wellington
  -175.2245   ,  // Pago Pago
  114.94171   ,  // Denpasar
  8.422979    ,  // Bern
  10.993931   ,  // Zurich
  30.908508   ,  // Cairo  // Current
  -4.06815    ,  // Abidjan  // Extension
  -0.11023331 ,  // Accra
  37.592777   ,  // Addis Ababa
  35.814497   ,  // Amman
  4.630920    ,  // Amsterdam
  176.08295   ,  // Anadyr
  33.485153   ,  // Ankara
  47.45643    ,  // Antananarivo
  57.15068    ,  // Ashgabat
  39.38273    ,  // Asmara
  69.13583    ,  // Astana
  -57.568623  ,  // Asunción
  -99.91861   ,  // Austin
  44.405907   ,  // Baghdad
  -77.2691    ,  // Baltimore
  -7.4586945  ,  // Bamako
  18.77937    ,  // Bangui
  16.022652   ,  // Bari
  -62.06457   ,  // Basse-Terre
  35.6966     ,  // Beirut
  20.778492   ,  // Belgrade
  -89.1536    ,  // Belize City
  74.30973    ,  // Bishkek
  -15.350592  ,  // Bissau
  11.665501   ,  // Bologna
  -71.529625  ,  // Boston
  -59.99299   ,  // Bridgetown
  152.74973   ,  // Brisbane
  4.843384    ,  // Brussels
  -17.204933  ,  // Canary Islands
  16.236038   ,  // Catanzaro
  -51.29435   ,  // Cayenne
  -80.401535  ,  // Charlotte
  29.152817   ,  // Chisinau
  -80.71135   ,  // Cleveland
  -82.554474  ,  // Columbus
  -13.336028  ,  // Conakry
  11.90641    ,  // Copenhagen
  -8.726114   ,  // Cork
  16.56650    ,  // Crotone
  -17.050703  ,  // Dakar
  36.925953   ,  // Damascus
  38.967915   ,  // Dar es Salaam
  130.80261   ,  // Darwin
  -83.20035   ,  // Detroit
  42.795224   ,  // Djibouti
  51.21495    ,  // Doha
  9.070695    ,  // Douala
  68.72686    ,  // Dushanbe
  -110.003825 ,  // Easter Island
  -3.5853018  ,  // Edinburgh
  -106.71078  ,  // El Paso
  11.223565   ,  // Florence
  -61.58718   ,  // Fort-de-France
  -13.138521  ,  // Freetown
  24.905453   ,  // Gaborone
  -90.96801   ,  // Galápagos Islands
  6.4023815   ,  // Geneva
  8.790691    ,  // Genoa
  -59.472546  ,  // Georgetown
  -37.7515    ,  // Grytviken
  -90.55627   ,  // Guatemala City
  -63.27070   ,  // Gustavia
  9.50757     ,  // Hamburg
  30.277515   ,  // Harare
  -83.8567    ,  // Havana
  147.23724   ,  // Hobart
  -96.56888   ,  // Houston
  -85.77939   ,  // Indianapolis
  27.735344   ,  // Johannesburg
  32.443825   ,  // Kampala
  32.52314    ,  // Khartoum
  -77.81739   ,  // Kingston
  47.465294   ,  // Kuwait
  2.4850502   ,  // Lagos
  -116.06636  ,  // Las Vegas
  14.633278   ,  // Ljubljana
  110.2       ,  // Haikou
  -84.88832   ,  // Louisville
  13.154247   ,  // Luanda
  27.479612   ,  // Lusaka
  7.62286     ,  // Luxembourg
  113.53685   ,  // Macau
  7.8199043   ,  // Malabo
  72.52665    ,  // Male
  -86.22636   ,  // Managua
  50.157827   ,  // Manama
  32.67328    ,  // Maputo
  -63.77564   ,  // Marigot
  40.772804   ,  // Mecca
  144.97224   ,  // Melbourne
  -90.59068   ,  // Memphis
  15.031148   ,  // Messina
  10.239992   ,  // Milan
  -88.31514   ,  // Milwaukee
  47.067223   ,  // Mogadishu
  -9.89758    ,  // Monrovia
  -55.61829   ,  // Montevideo
  -72.6222    ,  // Montréal
  12.483393   ,  // Munich
  58.42707    ,  // Muscat
  14.560964   ,  // Naples
  9.597755    ,  // Naters
  15.139576   ,  // N'Djamena
  1.033098    ,  // Niamey
  -16.244892  ,  // Nouakchott
  165.8780    ,  // Nouméa
  -179.46822  ,  // Nuku'alofa
  -50.35076   ,  // Nuuk
  135.52129   ,  // Osaka
  -2.3631992  ,  // Ouagadougou
  13.047504   ,  // Palermo
  -80.02137   ,  // Panama City
  -55.18518   ,  // Paramaribo
  60.103844   ,  // Perm
  -75.8900    ,  // Philadelphia
  19.405586   ,  // Podgorica
  -24.814996  ,  // Ponta Delgada
  57.3856     ,  // Port Louis
  -72.75783   ,  // Port-au-Prince
  -122.28293  ,  // Portland
  125.79389   ,  // Pyongyang
  95.71152    ,  // Yangon
  -35.16153   ,  // Recife
  -19.951889  ,  // Reykjavík
  48.669518   ,  // Riyadh
  55.426804   ,  // Saint-Denis
  -100.58327  ,  // San Antonio
  -117.3798   ,  // San Diego
  -121.42795  ,  // San Jose
  -84.57601   ,  // San José
  -66.522484  ,  // San Juan
  12.989004   ,  // San Marino
  -89.56902   ,  // San Salvador
  44.664364   ,  // Sana'a
  -70.08441   ,  // Santo Domingo
  121.3496    ,  // Shanghai
  114.113434  ,  // Shenzhen
  21.92619    ,  // Skopje
  -53.374178  ,  // St John's
  179.24705   ,  // Suva
  -152.05975  ,  // Tahiti
  -86.84646   ,  // Tegucigalpa
  106.51224   ,  // H? Chí Minh City
  117.1441    ,  // Tianjin
  7.8495617   ,  // Turin
  13.048315   ,  // Vaduz
  14.514596   ,  // Valletta
  11.261585   ,  // Verona
  55.05057    ,  // Victoria
  16.460888   ,  // Vienna
  -4.9830894  ,  // Yamoussoukro
  12.775005   ,  // Yaoundé
  16.518646   ,  // Zagreb
  112.75      ,  // Guangzhou
  -3.749494   ,  // Cardiff
  -104.30366  ,  // Regina
  -65.414894  ,  // Charlotte Amalie
  -83.44693   ,  // Atlanta
  44.849533   ,  // Mamoudzou
  -55.850253  ,  // St Pierre
  10.656972   ,  // Oslo
  154.60901   ,  // Srednekolymsk
  156.2654    ,  // Severo-Kurilsk
  114.266666  ,  // Wuhan
  104.064722  ,  // Chengdu
  108.9       ,  // Xi'an
  147.218806  ,  // Port moresby
  125.6       ,  // Davao City
  106.566667  ,  // Chongqing
  123.432253  ,  // Shenyang
  118.566667  ,  // Nanjing
  126.633333  ,  // Harbin
  116.983333  ,  // Jinan
  113.65      ,  // Zhengzhou
  102.683333  ,  // Kunming
  125.2       ,  // Changchun
  119.206389  ,  // Fuzhou
  109.845     ,  // Kuching
  119.416667  ,  // Makassar
  120.166667  ,  // Hangzhou
  120.483333  ,  // Qingdao
  108.316667  ,  // Nanning
  87.6           // Urumqi
};

float y_coordinate_billboard[] =
{
  28.72765    , // Midway
  20.50037    , // Honolulu
  18.416482   , // Hawaii
  66.82801    , // Alaska
  64.61519    , // Anchorage
  39.268623   , // San Francisco
  52.840466   , // Vancouver
  50.641914   , // Seattle
  36.0501     , // Los Angeles
  32.43412    , // Tijuana
  54.808723   , // Calgary
  34.421238   , // Phoenix
  24.850765   , // Mazatlan
  38.48978    , // Denver
  21.012651   , // Mexico City
  52.413305   , // Winnipeg
  -1.2958397  , // Diego Garcia
  33.75853    , // Dallas
  43.718916   , // Chicago
  46.393908   , // Toronto
  26.600098   , // Miami
  -14.47872   , // Lima
  -1.1588384  , // Quito
  40.064857   , // Washington DC
  48.360966   , // Ottawa
  42.91644    , // New York
  4.589889    , // Bogota
  -33.916725  , // Santiago
  -18.755084  , // La Paz
  9.479077    , // Caracas
  -38.301674  , // Buenos Aires
  57.073566   , // Newfoundland
  17.513548   , // St.John\'s
  -26.161484  , // Sao Paulo
  -15.362006  , // Brasilia
  -25.489334  , // Rio de Janeiro
  -4.632823   , // Fernando de Noronha
  40.92078    , // Azores
  41.237534   , // Lisbon
  35.55895    , // Casablanca
  56.3325     , // Dublin
  35.93782    , // Rabat
  42.94907    , // Madrid
  19.1149     , // The Settlement
  54.3643     , // London
  46.650515   , // Kingston
  43.945662   , // Barcelona
  51.232426   , // Paris
  38.8        , // Algiers
  38.464645   , // Tunis
  32.548077   , // Tripoli
  55.384163   , // Berlin
  44.30812    , // Rome
  52.390244   , // Prague
  -4.971325   , // Kinshasa
  62.984955   , // Stockholm
  50.357258   , // Bratislava
  -37.23294   , // Cape Town
  49.621403   , // Budapest
  55.11894    , // Warsaw
  57.96869    , // Kaliningrad
  44.761265   , // Sofia
  40.19036    , // Athens
  51.453014   , // Lviv
  63.798432   , // Helsinki
  59.39       , // Riga
  62.40095    , // Tallinn
  46.23181    , // Bucharest
  58.66338    , // Vilnius
  -13.020998  , // Lubumbashi
  56.074      , // Minsk
  63.66802    , // St. Petersburg
  43.30705    , // Istanbul
  59.0574     , // Kamchatka
  51.67283    , // Kiev
  33.40886    , // Jerusalem
  59.867274   , // Moscow
  51.871876   , // Kharkiv
  -2.28569216 , // Nairobi
  21.294516   , // Jeddah
  51.05744    , // Volgograd
  43.02292    , // Yerevan
  43.868797   , // Tbilisi
  42.724296   , // Baku
  56.249264   , // Chita
  55.40245    , // Samara
  37.827764   , // Tehran
  58.79702    , // Izhevsk
  25.920261   , // Abu Dhabi
  26.156519   , // Dubai
  58.625057   , // Ufa
  60.8112     , // Yekaterinburg
  58.67515    , // Chelyabinsk
  25.37315    , // Karachi
  36.247566   , // Kabul
  43.27668    , // Tashkent
  35.503504   , // Islamabad
  19.1539     , // Mumbai
  57.6478     , // Omsk
  47.071316   , // Almaty
  12.580097   , // Bangalore
  17.476889   , // Hyderabad
  30.229902   , // Delhi
  7.016384    , // Colombo
  12.973627   , // Chennai
  56.09611    , // Novosibirsk
  29.2487     , // Kathmandu
  54.396424   , // Novokuznetsk
  23.255802   , // Kolkata
  24.497862   , // Dhaka
  47.80117    , // Hovd
  57.70627    , // Krasnoyarsk
  13.954759   , // Bangkok
  2.1475217   , // Kuala Lumpur
  1.2819      , // Singapore
  55.907415   , // Irkutsk
  11.035363   , // Phnom Penh
  21.50975    , // Hanoi
  -7.6078978  , // Jakarta
  47.555336   , // Ulan Bator
  23.278207   , // Hong Kong
  -35.88091   , // Perth
  41.590416   , // Beijing
  14.53056    , // Manila
  25.610925   , // Taipei
  39.62031    , // Seoul
  -4.035683   , // Ambon
  45.714406   , // Vladivostok
  62.4232     , // Yakutsk
  51.60994    , // Khabarovsk
  -39.084026  , // Adelaide
  37.241367   , // Tokyo
  49.990294   , // Yuzhno-Sakhalinsk
  47.284135   , // Halifax
  13.5892     , // Guam
  -39.11797   , // Canberra
  62.78432    , // Magadan
  -37.15615   , // Sydney
  56.430623   , // Petropavlovsk
  2.4323818   , // Tarawa
  -37.709565  , // Auckland
  -42.79358   , // Wellington
  -16.50158   , // Pago Pago
  -9.839947   , // Denpasar
  49.32423    , // Bern
  49.956345   , // Zurich
  31.484703   , // Cairo <!-- Current
  4.79597     , // Abidjan <!-- Extension
  5.8756907   , // Accra
  9.030669    , // Addis Ababa
  33.384694   , // Amman
  55.7765     , // Amsterdam
  68.49775    , // Anadyr
  42.48622    , // Ankara
  -21.2698    , // Antananarivo
  41.36117    , // Ashgabat
  15.47704    , // Asmara
  50.53515    , // Astana
  -27.743135  , // Asunción
  31.163177   , // Austin
  34.704198   , // Baghdad
  40.53938    , // Baltimore
  13.68706    , // Bamako
  4.8865037   , // Bangui
  43.22443    , // Bari
  16.134456   , // Basse-Terre
  35.854736   , // Beirut
  46.721096   , // Belgrade
  17.484688   , // Belize City
  46.523335   , // Bishkek
  11.55114    , // Bissau
  46.283012   , // Bologna
  44.963028   , // Boston
  13.351773   , // Bridgetown
  -29.694848  , // Brisbane
  53.668407   , // Brussels
  29.933637   , // Canary Islands
  40.90       , // Catanzaro
  2.456624    , // Cayenne
  36.512573   , // Charlotte
  49.921635   , // Chisinau
  43.748566   , // Cleveland
  41.756615   , // Columbus
  8.631499    , // Conakry
  58.891415   , // Copenhagen
  55.403093   , // Cork
  41.66904    , // Crotone
  15.208498   , // Dakar
  34.746265   , // Damascus
  -8.2424073  , // Dar es Salaam
  -14.162645  , // Darwin
  44.547348   , // Detroit
  12.1866785  , // Djibouti
  26.597656   , // Doha
  3.6997486   , // Douala
  40.787346   , // Dushanbe
  -30.22477   , // Easter Island
  59.428947   , // Edinburgh
  32.146854   , // El Paso
  45.662277   , // Florence
  14.76892    , // Fort-de-France
  7.655516    , // Freetown
  -27.248528  , // Gaborone
  -1.5199649  , // Galápagos Islands
  48.21951    , // Geneva
  47.321915   , // Genoa
  6.380968    , // Georgetown
  -57.7054    , // Grytviken
  14.634536   , // Guatemala City
  18.33953    , // Gustavia
  56.77807    , // Hamburg
  -19.850468  , // Harare
  23.343322   , // Havana
  -47.08036   , // Hobart
  30.260753   , // Houston
  41.51259    , // Indianapolis
  -27.906626  , // Johannesburg
  -0.4070666  , // Kampala
  17.215006   , // Khartoum
  18.41480    , // Kingston
  30.985881   , // Kuwait
  6.227817    , // Lagos
  37.31196    , // Las Vegas
  48.189632   , // Ljubljana
  20.43       , // Haikou
  38.988453   , // Louisville
  -10.58106   , // Luanda
  -17.50159   , // Lusaka
  52.136436   , // Luxembourg
  23.145636   , // Macau
  2.9978683   , // Malabo
  5.0075955   , // Male
  11.278207   , // Managua
  27.348265   , // Manama
  -28.068464  , // Maputo
  18.457706   , // Marigot
  20.819624   , // Mecca
  -41.70471   , // Melbourne
  37.715267   , // Memphis
  40.16947    , // Messina
  47.91952    , // Milan
  46.1329     , // Milwaukee
  3.4613315   , // Mogadishu
  5.5622296   , // Monrovia
  -38.18476   , // Montevideo
  48.099754   , // Montréal
  51.55925    , // Munich
  24.604519   , // Muscat
  42.783074   , // Naples
  48.82047    , // Naters
  11.65933    , // N'Djamena
  14.355632   , // Niamey
  18.300085   , // Nouakchott
  -24.376561  , // Nouméa
  -25.497614  , // Nuku'alofa
  67.97327    , // Nuuk
  36.21724    , // Osaka
  13.90666    , // Ouagadougou
  39.933194   , // Palermo
  8.850155    , // Panama City
  5.3009357   , // Paramaribo
  62.749435   , // Perm
  41.64759    , // Philadelphia
  44.589222   , // Podgorica
  40.089258   , // Ponta Delgada
  -22.37477   , // Port Louis
  18.809178   , // Port-au-Prince
  47.340336   , // Portland
  40.81527    , // Pyongyang
  16.876668   , // Yangon
  -9.211591   , // Recife
  67.71701    , // Reykjavík
  25.639841   , // Riyadh
  -23.425283  , // Saint-Denis
  29.915714   , // San Antonio
  33.52623    , // San Diego
  38.167816   , // San Jose
  9.7441225   , // San José
  18.66414    , // San Juan
  46.086857   , // San Marino
  13.904434   , // San Salvador
  16.159756   , // Sana'a
  19.15683    , // Santo Domingo
  32.475516   , // Shanghai
  24.675684   , // Shenzhen
  44.16372    , // Skopje
  50.063976   , // St John's
  -23.705237  , // Suva
  -19.160411  , // Tahiti
  14.716576   , // Tegucigalpa
  10.947448   , // H? Chí Minh City
  41.42634    , // Tianjin
  47.352306   , // Turin
  49.244553   , // Vaduz
  37.34911    , // Valletta
  48.416846   , // Verona
  -5.83902    , // Victoria
  51.02616    , // Vienna
  7.987684    , // Yamoussoukro
  3.8569666   , // Yaoundé
  47.46668    , // Zagreb
  24.36       , // Guangzhou
  54.693363   , // Cardiff
  55.208904   , // Regina
  18.718129   , // Charlotte Amalie
  34.03972    , // Atlanta
  -14.4259    , // Mamoudzou
  49.67525    , // St Pierre
  62.91596    , // Oslo
  72.466667   , // Srednekolymsk
  53.483333   , // Severo-Kurilsk
  30.583333   , // Wuhan
  30.658611   , // Chengdu
  34.266667   , // Xi'an
  -9.513611   , // Port moresby
  7.066667    , // Davao City
  29.558333   , // Chongqing
  43.305865   , // Shenyang
  33.25       , // Nanjing
  48.15       , // Harbin
  38.166667   , // Jinan
  34.766667   , // Zhengzhou
  24.066667   , // Kunming
  46.4        , // Changchun
  26.576111   , // Fuzhou
  1.06        , // Kuching
  -6.133333   , // Makassar
  30.25       , // Hangzhou
  37.766667   , // Qingdao
  22.816667   , // Nanning
  47.125        // Urumqi
};

Vector3 timezoneSelColorPart1[37] =
{
  Vector3(1.0f, 1.0f, 0.0f), Vector3(1.0f, 0.0f, 1.0f), Vector3(0.0f, 1.0f, 1.0f),
  Vector3(1.0f, 0.0f, 0.0f), Vector3(0.0f, 1.0f, 0.0f), Vector3(0.0f, 0.0f, 1.0f),
  Vector3(1.0f, 1.0f, 0.0f), Vector3(1.0f, 0.0f, 1.0f), Vector3(0.0f, 1.0f, 1.0f),
  Vector3(1.0f, 0.0f, 0.0f), Vector3(0.0f, 1.0f, 0.0f), Vector3(0.0f, 0.0f, 1.0f),
  Vector3(1.0f, 1.0f, 0.0f), Vector3(1.0f, 0.0f, 1.0f), Vector3(0.0f, 1.0f, 1.0f),
  Vector3(1.0f, 0.0f, 0.0f), Vector3(0.0f, 1.0f, 0.0f), Vector3(0.0f, 0.0f, 1.0f),
  Vector3(1.0f, 1.0f, 0.0f), Vector3(1.0f, 0.0f, 1.0f), Vector3(0.0f, 1.0f, 1.0f),
  Vector3(1.0f, 0.0f, 0.0f), Vector3(0.0f, 1.0f, 0.0f), Vector3(0.0f, 0.0f, 1.0f),
  Vector3(1.0f, 1.0f, 0.0f), Vector3(1.0f, 0.0f, 1.0f), Vector3(0.0f, 1.0f, 1.0f),
  Vector3(1.0f, 0.0f, 0.0f), Vector3(0.0f, 1.0f, 0.0f), Vector3(0.0f, 0.0f, 1.0f),
  Vector3(1.0f, 1.0f, 0.0f), Vector3(1.0f, 0.0f, 1.0f), Vector3(0.0f, 1.0f, 1.0f),
  Vector3(1.0f, 0.0f, 0.0f), Vector3(0.0f, 1.0f, 0.0f), Vector3(0.0f, 0.0f, 1.0f),
  Vector3(1.0f, 1.0f, 0.0f)
};

Vector3 timezoneSelColorPart2[37] =
{
  Vector3(0.0f, 0.0f, 1.0f), Vector3(0.0f, 0.0f, 1.0f), Vector3(0.0f, 0.0f, 1.0f),
  Vector3(0.0f, 0.0f, 1.0f), Vector3(0.0f, 0.0f, 1.0f), Vector3(0.0f, 0.0f, 1.0f),
  Vector3(1.0f, 1.0f, 0.0f), Vector3(1.0f, 1.0f, 0.0f), Vector3(1.0f, 1.0f, 0.0f),
  Vector3(1.0f, 1.0f, 0.0f), Vector3(1.0f, 1.0f, 0.0f), Vector3(1.0f, 1.0f, 0.0f),
  Vector3(1.0f, 0.0f, 1.0f), Vector3(1.0f, 0.0f, 1.0f), Vector3(1.0f, 0.0f, 1.0f),
  Vector3(1.0f, 0.0f, 1.0f), Vector3(1.0f, 0.0f, 1.0f), Vector3(1.0f, 0.0f, 1.0f),
  Vector3(0.0f, 1.0f, 1.0f), Vector3(0.0f, 1.0f, 1.0f), Vector3(0.0f, 1.0f, 1.0f),
  Vector3(0.0f, 1.0f, 1.0f), Vector3(0.0f, 1.0f, 1.0f), Vector3(0.0f, 1.0f, 1.0f),
  Vector3(1.0f, 0.0f, 0.0f), Vector3(1.0f, 0.0f, 0.0f), Vector3(1.0f, 0.0f, 0.0f),
  Vector3(1.0f, 0.0f, 0.0f), Vector3(1.0f, 0.0f, 0.0f), Vector3(1.0f, 0.0f, 0.0f),
  Vector3(0.0f, 1.0f, 0.0f), Vector3(0.0f, 1.0f, 0.0f), Vector3(0.0f, 1.0f, 0.0f),
  Vector3(0.0f, 1.0f, 0.0f), Vector3(0.0f, 1.0f, 0.0f), Vector3(0.0f, 1.0f, 0.0f),
  Vector3(0.0f, 0.0f, 1.0f)
};

class HelloWorldController : public ConnectionTracker
{
public:

  HelloWorldController(Application& application)
  : mApplication(application)
  , mRadius(150.f)
  , mCurrentTimeZone(0)
  {
    // Connect to the Application's Init signal
    mApplication.InitSignal().Connect(this, &HelloWorldController::Create);
  }

  ~HelloWorldController()
  {
    // Nothing to do here;
  }

  Dali::ShaderEffect NewEffect()
  {
    std::string vertexShader = DALI_COMPOSE_SHADER(
      precision highp float;

      uniform vec3  uLightDirection;

      varying vec2  vTextureCoords1;
      varying vec2  vTextureCoords2;
      varying float vBrightness;
      varying float vInRangeForTZHighlight;

      void main()
      {
        float PI = 3.141592;

        vTextureCoords1 = vec2(1.0 - aTexCoord.x, aTexCoord.y / 2.0);
        vTextureCoords2 = vec2(vTextureCoords1.x, vTextureCoords1.y + 0.5);

        mediump vec3 normal = normalize(aPosition.xyz);
        vBrightness = sqrt(1.0 - dot(uLightDirection, normal)) * 4.0;

        gl_Position = uMvpMatrix * vec4(aPosition.xyz, 1.0);
        vNormal = uModelViewIT * aNormal;

        /*
        float sectors = 30.0;
        float S = 1.0 / (sectors - 1.0);
        float s = aTexCoord.x * sectors;

        mediump vec2 LatLong;
        LatLong.x = (2.0 * PI * s * S) - PI;

        mediump vec2 timezoneLatLong = vec2(1.0 / 22.0 * PI, 0.0);
        float angleDelta = mod(abs(timezoneLatLong.x - LatLong.x), 6.28);
        vInRangeForTZHighlight = step(3.14/2.0, min(angleDelta, 6.28-angleDelta));
        */
      }
    );

    std::string fragmentShader = DALI_COMPOSE_SHADER(
      precision highp float;

      uniform vec3  uTimezoneColorUpper;
      uniform vec3  uTimezoneColorLower;

      varying vec2  vTextureCoords1;
      varying vec2  vTextureCoords2;
      varying float vBrightness;
      varying float vInRangeForTZHighlight;

      void main()
      {
        float timezoneVisibility = 1.0;

        mediump vec4 dayColorUpper = texture2D(sTexture, vTextureCoords1);
        mediump vec4 dayColorLower = texture2D(sTexture, vTextureCoords2);
        mediump vec3 stepUpper = step(0.5, dayColorUpper.rgb);
        mediump vec3 stepLower = step(0.5, dayColorLower.rgb);

        mediump vec3 dayColorUpperQuantized = abs(stepUpper - uTimezoneColorUpper);
        mediump vec3 dayColorLowerQuantized = abs(stepLower - uTimezoneColorLower);
        float flag = clamp(0.0/*vInRangeForTZHighlight*/ + dayColorUpperQuantized.r+dayColorUpperQuantized.g+dayColorUpperQuantized.b+dayColorLowerQuantized.r+dayColorLowerQuantized.g+dayColorLowerQuantized.b, 0.0, 1.0);

        float continents = clamp((stepUpper.r+stepUpper.g+stepUpper.b)*(stepLower.r+stepLower.g+stepLower.b), 0.0, 1.0);
        mediump vec4 tempColor = continents*flag*vec4(0.96, 0.96, 0.96, 1.0) + (vec4(0.965, 0.882, 0.557, 1.0)*timezoneVisibility + continents*vec4(0.96, 0.96, 0.96, 1.0)*(1.0-timezoneVisibility))*(1.0-flag);
        mediump vec4 dayColor = mix(vec4(0.498, 0.765, 0.894, 1.0), tempColor, continents);

        float nightColor = 0.65 * 0.15;

        gl_FragColor = vec4(dayColor.rgb*(0.8 + 0.2) - vBrightness * nightColor, 1.0);
      }
    );

    Dali::ShaderEffect shaderEffect = Dali::ShaderEffect::New(vertexShader,
        fragmentShader, Dali::GeometryType(Dali::GEOMETRY_TYPE_TEXTURED_MESH),
        Dali::ShaderEffect::GeometryHints(Dali::ShaderEffect::HINT_NONE));

    return shaderEffect;
  }

  Mesh CreateMesh()
  {
    // We create the meshdata for the metaballs
    Stage stage = Stage::GetCurrent();

    // Create vertices and specify their color
    float fPI   = (float) M_PI;
    int rings   = 30;
    int sectors = 30;
    float R     = 1.0f / (float) (rings - 1);
    float S     = 1.0f / (float) (sectors - 1);

    MeshData::VertexContainer vertices(rings * sectors);

    for (int r = 0; r < rings; r++)
    {
      for (int s = 0; s < sectors; s++)
      {
        float y = (float) sinf(-fPI * 0.5f + fPI * r * R);
        float x = (float) cosf(2 * fPI * s * S) * (float) sinf(fPI * r * R);
        float z = (float) sinf(2 * fPI * s * S) * (float) sinf(fPI * r * R);

        vertices[r * sectors + s] = MeshData::Vertex(Vector3(x * mRadius * -1, y * mRadius * -1, z * mRadius), // x, y, z
                                                     Vector2((float) s * S, (float) r * R), // u, v
                                                     Vector3(x, y, z)); // nx, ny, nz
      }
    }

    MeshData::FaceIndices faces(rings * sectors * 6);
    for (int r = 0; r < rings - 1; r++)
    {
      for (int s = 0; s < sectors - 1; s++)
      {
        faces.push_back((short) (r * sectors + s));
        faces.push_back((short) (r * sectors + (s + 1)));
        faces.push_back((short) ((r + 1) * sectors + s));

        faces.push_back((short) ((r + 1) * sectors + s));
        faces.push_back((short) (r * sectors + (s + 1)));
        faces.push_back((short) ((r + 1) * sectors + (s + 1)));
      }
    }

    Material baseMaterial = Material::New("Material1");
    baseMaterial.SetDiffuseColor(Vector4(1.0f, 1.0f, 1.0f, 1.0f));

    mWorldmapImage = ResourceImage::New(WORLDMAP_IMAGE);
    baseMaterial.SetDiffuseTexture(mWorldmapImage);

    // Create the mesh data from the vertices and faces
    MeshData meshData;
    meshData.SetMaterial(baseMaterial);
    meshData.SetVertices(vertices);
    meshData.SetFaceIndices(faces);
    meshData.SetHasTextureCoords(true);
    meshData.SetHasNormals(true);

    // Create a mesh from the data
    Dali::Mesh mesh = Mesh::New(meshData);
    return mesh;
  }

  // The Init signal is received once (only) during the Application lifetime
  void Create(Application& application)
  {
    // Get a handle to the stage
    Stage stage = Stage::GetCurrent();
    Size stageSize = stage.GetSize();

    stage.SetBackgroundColor(Color::BLACK);

    /**
     * Globe model
     */
    mGlobeActor = MeshActor::New(CreateMesh());
    mGlobeActor.SetName("mesh actor");
    mGlobeActor.SetParentOrigin(ParentOrigin::CENTER);
    mGlobeActor.SetAnchorPoint(AnchorPoint::CENTER);

    mGlobeRotation  = Quaternion(Radian(Math::PI_2), Vector3::YAXIS);
    mGlobeRotation *= Quaternion(Radian(Math::PI),   Vector3::ZAXIS);

    mGlobeActor.SetOrientation(mGlobeRotation);

    stage.Add(mGlobeActor);

    Vector3 lightDirection(0.f, 0.f, 1.f);
    lightDirection *= Quaternion(Radian(Math::PI_2), Vector3::YAXIS);

    ShaderEffect effect = NewEffect();
    mCurrentTimeZone = 14;
    effect.SetUniform(LIGHT_DIRECTION_PROPERTY_NAME, lightDirection);
    effect.SetUniform(TIMEZONE_COLOR_UPPER_PROPERTY_NAME, timezoneSelColorPart1[mCurrentTimeZone]);
    effect.SetUniform(TIMEZONE_COLOR_LOWER_PROPERTY_NAME, timezoneSelColorPart2[mCurrentTimeZone]);
    mGlobeActor.SetShaderEffect(effect);

    /**
     * City
     */

    //int index = 59-34;  // New york
    //int index = 55-34;  // Lima
    int index = 159-34;   // Seoul

    float latitude = x_coordinate_billboard[index] * 3.14f / 180.f;
    float longitude = y_coordinate_billboard[index] * 3.14f / 180.f;

    Vector3 cityLocation;
    cityLocation.x = (float)cos(latitude) * (float)cos(longitude) * (mRadius + 0);
    cityLocation.y = (float)sin(longitude) * (mRadius + 0);
    cityLocation.z = (float)sin(latitude) * (float)cos(longitude) * (mRadius + 0);

#if 1
    Actor cityActor = CreateSolidColorActor(Color::RED);
    cityActor.SetParentOrigin(ParentOrigin::CENTER);
    cityActor.SetAnchorPoint(AnchorPoint::CENTER);
    cityActor.SetSize(10.f, 10.f);
    cityActor.SetPosition(cityLocation);
    mGlobeActor.Add(cityActor);
#endif

#if !(TOUCH_ENABLED)
    Animation anim = Animation::New(6.f);
    anim.SetLooping(true);

#if 0
    mGlobeActor.SetScale(1.5f, 1.5f, 1.5f);
    anim.AnimateBy(Property(mGlobeActor, Actor::Property::ORIENTATION), Quaternion(Radian(Math::PI * 2.f), Vector3::YAXIS));
#else
    Vector3 worldXAxis = Vector3::XAXIS;
    worldXAxis *= Quaternion(Radian(Math::PI_2 + latitude), Vector3::YAXIS);
    worldXAxis *= Quaternion(Radian(Math::PI), Vector3::ZAXIS);

    mGlobeRotation *= Quaternion(Radian(latitude), Vector3::YAXIS);
    mGlobeRotation *= Quaternion(Radian(longitude), worldXAxis);

    anim.AnimateTo(Property(mGlobeActor, Actor::Property::ORIENTATION), mGlobeRotation, AlphaFunction::BOUNCE);
    anim.AnimateTo(Property(mGlobeActor, Actor::Property::SCALE), Vector3(2.5f, 2.5f, 2.5f), AlphaFunction::BOUNCE);
#endif
    anim.Play();
#else
    mGlobeActor.SetScale(1.5f, 1.5f, 1.5f);
#endif

    mGlobeActor.SetSensitive(false);

    // Add event handling for touch and pan gesture
    Actor eventActor = Actor::New();
    eventActor.SetParentOrigin(ParentOrigin::CENTER);
    eventActor.SetAnchorPoint(AnchorPoint::CENTER);
    eventActor.SetSize(stageSize);
    stage.Add(eventActor);
    eventActor.TouchedSignal().Connect(this, &HelloWorldController::OnTouched);

#if TOUCH_ENABLED
    mPanDetector = PanGestureDetector::New();
    mPanDetector.DetectedSignal().Connect(this, &HelloWorldController::OnPan);
    mPanDetector.Attach(eventActor);
#endif
  }

  Quaternion GetHorizontalRotation(float diffHorizontal, float& diffAccumulated)
  {
    diffAccumulated += diffHorizontal;

    mGlobeRotation *= Quaternion(Radian(-diffHorizontal / (Math::PI * 32.f)), Vector3::YAXIS);

    return mGlobeRotation;
  }

  Quaternion GetVerticalRotation(float diffVertical, float& diffAccumulated)
  {
    diffAccumulated += diffVertical;

    if (diffAccumulated > 60.f)
    {
      diffAccumulated = 61.f;
    }
    else if (diffAccumulated < -60.f)
    {
      diffAccumulated = -61.f;
    }

    Vector3 worldXAxis = Vector3::XAXIS;
    worldXAxis *= mGlobeRotation;

    return (mGlobeRotation * Quaternion(Radian(-diffAccumulated / (Math::PI * 32.f)), worldXAxis));
  }

  bool OnTouched(Actor actor, const TouchEvent& event)
  {
#if TOUCH_ENABLED
    TouchPoint touchPoint = event.GetPoint(0);

    Quaternion localRotation = mGlobeRotation;

    if (TouchPoint::Down == touchPoint.state)
    {
      mTouchedPosition = touchPoint.screen;

      if (mGlobeRotateAnimation)
      {
        mGlobeRotateAnimation.Stop();
        mGlobeRotateAnimation.Reset();
      }
    }
    else if (TouchPoint::Motion == touchPoint.state)
    {
      Vector2 diff = touchPoint.screen - mTouchedPosition;

      localRotation = GetHorizontalRotation(diff.x, mDiffAccumulated.x);
      localRotation = GetVerticalRotation(diff.y, mDiffAccumulated.y);

      mGlobeActor.SetOrientation(localRotation);
      mTouchedPosition = touchPoint.screen;
    }

#else
    // quit the application
    mApplication.Quit();
#endif

    return true;
  }

  void OnPan(Actor actor, const Dali::PanGesture& gesture)
  {
    switch( gesture.state )
    {
      case Gesture::Finished:
      case Gesture::Cancelled:
      {
        Vector2 dir;
        float length = sqrtf(gesture.velocity.x * gesture.velocity.x + gesture.velocity.y * gesture.velocity.y) * 16.f;
        if (gesture.velocity.x)
        {
          dir.x = gesture.velocity.x / fabsf(gesture.velocity.x) * length;
        }
        if (gesture.velocity.y)
        {
          dir.y = gesture.velocity.y / fabsf(gesture.velocity.y) * length;
        }

        Quaternion localRotation;
        localRotation = GetHorizontalRotation(dir.x, mDiffAccumulated.x);
        localRotation = GetVerticalRotation(dir.y, mDiffAccumulated.y);

        mGlobeRotateAnimation = Animation::New(1.5f);
        mGlobeRotateAnimation.AnimateTo(Property(mGlobeActor, Actor::Property::ORIENTATION), localRotation, AlphaFunction::EASE_OUT);
        mGlobeRotateAnimation.Play();

        break;
      }

      default:
        break;
    }
  }

private:
  Application&  mApplication;

  Image         mWorldmapImage;

  PanGestureDetector mPanDetector;

  Animation     mGlobeRotateAnimation;
  MeshActor     mGlobeActor;
  Quaternion    mGlobeRotation;

  Vector2       mDiffAccumulated;
  Vector2       mTouchedPosition;

  float         mRadius;
  int           mCurrentTimeZone;

};

void RunTest( Application& application )
{
  HelloWorldController test( application );

  application.MainLoop();
}

// Entry point for Linux & Tizen applications
//
int main( int argc, char **argv )
{
  Application application = Application::New( &argc, &argv );

  RunTest( application );

  return 0;
}
