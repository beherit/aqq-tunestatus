2.2.1.0
-----
* Obsługa natywnego stylu Windows (wyłączone skórkowanie okien wtyczki).
* Poprawne ustawianie domyślnej listy obsługiwanych odtwarzaczy.
* Aktualizacja komponentów AlphaControls do najnowszej dostępnej wersji (7.61).
* Inne mniej ważne poprawki optymalizacyjne.

2.2.0.0
-----
* Pełne skórkowanie okna ustawień wtyczki.
* Wsparcie wtyczki AQQ Streamer.
* Poprawiono obsługę Scrobbler Last.fm.
* Zmiany w strukturze pliku ustawień wtyczki.
* Znaczna optymalizacja kodu.

2.1.0.2
-----
* Poprawiono obsługę Scrobbler Last.fm.

2.1.0.0
-----
* Rozszerzenie obsługi XEP-0118 o długość słuchanego utworu (działa tylko z WMP oraz Winamp/AIMP).
* Poprawiono obsługę Screamer Radio w Windows 7.
* Zmiana adresów URL do wymaganych dodatków dla odtwarzaczy WMP, Foobar2000 oraz iTunes.
* Poprawki wizualne formy ustawień.
* Drobne poprawki w kodzie.

2.0.2.0
-----
* Zabezpieczenie przed bugiem AQQ nr. 2941.
* Po przejściu w stan niewidoczny/rozłączony początkowy opis jest poprawnie przywracany.
* Poprawki kosmetyczne formy ustawień.
* Optymalizacja całego kodu wtyczki.
* Zmiana sposobu pobierania odtwarzanego utworu w Windows Media Player.
* Usunięcie tagu CC_SAMPLERATE, CC_BITRATE oraz CC_CHANNELS.
* Tag CC_SONGLENGTH działa również dla Windows Media Player (tylko przy pobieraniu informacji nowym sposobem).

2.0.1.0
-----
* Przepisanie wtyczki pod kompilator Embarcadero C++ Builder 2010.
* Poprawki kosmetyczne wyglądu formy.
* Optymalizacja kodu.
* Prawie całkowite rozdzielenie w kodzie obsługi User Tune z ustawianiem opisu.
* Poprawa w notyfikacji UserTune.
* Możliwość dodania osób do listy wyjątków, które będą wyłączone z notyfikacji o aktualnie odtwarzanym przez nich utworu muzycznego.
* Usuwanie z pobranego utworu ".mp3", ".wma", ".avi", ".mpg", ".mpeg", ".rmvb".
* Możliwość wyłączenia zabezpieczenia przed pobieraniem aktualnie odtwarzanych filmów wideo.
* Naprawa pobierania aktualnie odtwarzanego utworu w aTunes.
* Poprawka w wycinaniu adresów stron WWW z pobranego utworu.

2.0.0.0
-----
* Przepisanie wtyczki pod kompilator CodeGear C++ Builder 2009.
* Pełne wsparcie Unicode!
* Zabezpieczenie przed ustawianiem opisu/wysyłaniu UserTune gdy w pobranym tekście okna znajdzie się .avi, .mpg, .mpeg lub .rmvb.
* Naprawiono poważny błąd w przywracaniu/pobieraniu opisu.
* Bitrate jest teraz poprawnie wyświetlane tzn. "wartość kbps".
* Zwiększono limit znaków w sprawdzaniu ciągu znaków przy ucinaniu numeru utworów.
* Poprawiono obsługę XEP-0118.
* Gdy wymusimy działanie UserTune z odtwarzaczami obsługiwanymi przez AQQ to wtyczka wyłączy obsługę UserTune w AQQ.
* Minimalny czas po, którym ustawiany jest opis/wysyłana informacja o słuchanym utworze to 4 sekundy.
* Mała poprawki w obsłudze Screamer Radio, aTunes, Songbird, Last.fm Player, iTunes.
* Zwiększono limit czasowy po, którym są pokazywane chmurki z informacją o aktualnie słuchanych utworach naszych znajomych po włączeniu AQQ.
* Poprawka w blokowaniu zmiany opisu podczas aktywnego działania wtyczki.

1.0.6.0
-----
* Poprawienie linków do wymaganych dodatków.
* Dropna poprawka w kodzie ustawiania opisu.
* Poprawka obsługi aTunes.
* Dodanie obsługi XEP-0118 (User Tune):
 -Notyfikacja o słuchanych utworach przez naszych znajomych.
 -Wysyłanie tego co słuchamy.
 
 1.0.5.4
-----
* Przy aktywnej opcji nie ustawiania opisu przy stanie niewidoczny po wejściu w ten stan opis początkowy jest automatycznie przywracany.
* Poprawiono błąd z zawieszaniem AQQ przy wycinaniu linków z pobranego utworu.
* Poprawiono pobieranie utworu z Screamer Radio.

1.0.5.2
-----
* Dodano możliwość ustawienia czy wtyczka ma czy też nie ustawiać opis przy stanie niewidoczny.
* Checkbox opcji wyłączania działania wtyczki gdy utwór nie zmienia się od X minut działa już normalnie.

1.0.5.0
-----
* Dodanie obsługi aTunes.
* Dodanie obsługi Screamer Radio.
* Zmiana wyglądu karty "Obsługa".
* Dodanie Drag&Drop dla listy odtwarzaczy.
* Usunięcie SpeedButton'ów do zmiany kolejności odtawarzaczy.
* Dodono opcję wycinania z pobranego utworu adresów stron WWW (domyślnie wyłączone).
* Wtyczka nie ustawia opisu gdy stan głównego konta jabbera jest niewidoczny bądź rozłączony.
* Dodano opcję wyłączania działania wtyczki gdy utwór nie zmienia się od X minut (domyślnie wyłączone).

1.0.4.18
-----
* Optymalizacja działania dla Last.fm Player i Songbird.
* Domyślnie Last.fm Player i Songbird są wyłączone z automatycznego trybu.
* Inne poprawki w kodzie.

1.0.4.16
-----
* Bardzo znacząca optymalizacja kodu.
* Usunięto tag CC_PLAYERNAME - sprawiał problemy.
* Dodanie obsługi Songbird.
* Dodanie menu pod PPM dla odtwarzaczy wymagających do obsługi dodatkowych rozszerzeń w programie dzięki, którym można je szybko ściągnąc.
* Dodanie możliwości zdefiniowania trybu automatycznego - można tam ustawić hierarchie sprawdzania jak i wykluczyć dane odtwarzacze ze sprawdzania.

1.0.4.14
-----
* Poprawka przy pobieraniu danych z wtyczki AQQ Radio i funkcji ucinania nazwy radiostacji.
* Dodano opcję wyłączającą funkcję ustawiania opisu po danym czasie od rozpoczęcia utworu dla pobierania danych z wtyczki AQQ Radio.
* Poprawiono błąd z ustawianiem pustego opisu po zapisaniu ustawień gdy wtyczka nie była włączona.
* Poprawka w pobieraniu opisu początkowego gdy w między czasie zmienimy go sami.
* Scalenie obsługi WMP 6 i 7-11.
* Poprawiono drobny wyciek pamięci.

1.0.4.12
-----
* Poprawienie działania funkcji po jakim czasie od rozpoczęcia utworu ma zmieniać się opis.

1.0.4.11
-----
* Dodanie opcji ucinania nazwy radiostacji z pobieranego utworu z wtyczki AQQ Radio (domyślnie włączone).

1.0.4.10
-----
* Usunięcie w zakładce "Obsługa" label'ów informujących i zastąpienie ich hint'ami.
* Poprawka przy pobieraniu opisu z Last.fm.
* Dodanie obsługi wtyczki AQQ Radio!
* Dodanie obsługi ALSong.
* Zmiana w ustawieniach zaznaczenia dla "Pokazuj przycisk szybkiego włączenia/wyłączenia" zachodzi dopiero po zapisaniu ustawień.

1.0.4.8
-----
* Dodanie zabezpieczenia przed wpisywaniem tych samych tagów.
* Mała poprawka przy pobieraniu opisu z Foobar2000 gdy używana jest wtyczka fooAvA.
* Dodanie obługi iTunes (wymagany plugin do działania)!
* Dodanie dodatkowego sposobu pobierania informacji o utworze z Foobar2000 (który jest przydatny przy chowaniu Foobar'a do tray'a) - aby z niego skorzystać należy zainstalować wtyczkę do Foobar2000 autostwa MentosX i odpowiednio ją skonfigurować.
* Optymalizacja kodu.

1.0.4.6
-----
* Poprawienie działania funkcji po jakim czasie od rozpoczęcia utworu ma zmieniać się opis.

1.0.4.5
-----
* Mała poprawka przy pobieraniu opisu z Foobar2000 gdy używa się wtyczki fooAvA.

1.0.4.4
-----
* Poprawki w przywracaniu opisu podczas wyładowywania wtyczki.
* Mała poprawka przy pobieraniu opisu z Last.fm.
* Dodano funkcję po jakim czasie od rozpoczęcia utworu ma zmieniać się opis (przydatne przy wyszukiwaniu utworu w odtwarzaczu, domyślna wartość ustawiona na 5 sekund).

1.0.4.2
-----
* Usunięto tag CC_PLAYERVERSION - jednak jakoś taki nie potrzebny był ;)
* Drobne poprawki w kodzie przy CC_PLAYERNAME.
* Zmiana opisu w ustawieniach jest brana pod uwagę dopiero po zapisie - opis nie jest już zmieniany "na żywo" podczas edycji.
* Gdy w opisie nie ma tagu CC_TUNESTATUS to przycisk OK jest wyłączony. 
* Drobne poprawki w ustawianiu opisu (m.in. ucinanie spacji gdy opis początkowy był pusty a w ustawianym opisie był tag CC_STARTSTATUS).
* Poprawienie opcji ustawiaj opisu tylko na kontach sieci Jabber - jak się zaznaczy i zapisze zmiany to w innych kontach jest przywracany opis początkowy.

1.0.4.0
-----
* Poprawiono sposób pobierania tytułu z LastFM - od teraz nie ma szans, żeby jakieś inne okno się wdrało do opisu ;)
* Poprawki optymalizujące w pobieraniu utworów.
* Ustawienia nie są zapisywane już po zamknięciu formy przez X.
* Ikonka szybkiego włączenia/wyłączenia działania wtyczki jest teraz domyślnie włączona.
* Poprawiono wygląd wtyczki.
* Dodano obsługę tagów! Co za tym idzie wywalono pola Prefix i Suffix - teraz wygląd ustawia się jak przy normalnej zmianie opisu (pole Memo).

1.0.3.6
-----
* Ucinanie numerów piosenki jest domyślnie włączone, opcja zniknęła z ustawień.
* Za nową linię odpowiada %n.
* Zmiany w odczycie/zapisie prefix/suffix.
* Poprawki w pozostawianiu opisu po wtyczce podczas jej wyłączania/wyładowywania.
* Dodanie ostrzeżenia gdy AQQ nie spełnia wymagań co do wersji.
* Inne drobne poprawki w kodzie.

1.0.3.4
-----
* Poprawiono błąd ze wstawianiem do opisu "Nokia PC Suite" i "PCSuite".
* Poprawiono błąd z odczytywaniem ustawień, który niekiedy występował.
* Pliki wtyczki tworzone są w katalogu TuneStatus w profilu prywatnym - bo bałagan się robił ;)

1.0.3.2
-----
* Poprawiono błąd z niepokazywaniem grafiki ikonki szybkie włączania/wyłączania wtyczki.

1.0.3.0
-----
* Przebudowa wyglądu wtyczki.
* Poprawiono błąd gdy nie mieliśmy włączonej muzyki a wtyczka została włączona to i tak zmieniała opis we wszystkich sieciach względem głównego jabbera - teraz wtyczka nie zmiania w takiej sytuacji w ogóle opisu.
* Dodano informację o obsłudze KMPlayer (btw. jak komuś pokazuje w opisie np "[4/60] Artysta - tytuł" to niech ustawi w opcjach sobie wyświetlanie i będzie miał bez numeru - to samo tyczy się innych odtwarzaczy).
* Zwiększono sprawdzanie tytułu w odtwarzaczach z 1 sekundy do 3 ze względu na KMPlayer.
* Dodano opcje ucinania numerów piosenki (np. z: 16. Artysta - Tytuł) - jako opcja bo nie wszystkie odtwarzacze mają numer piosenki w utworze i niekiedy wystarczy tylko to ustawić a tak to mogło by usuwać  nazwę utworu gdzie znalazły by się znaki ". " ;).
* Dodano opcje ustawiania opisu tylko na kontach sieci Jabber (na ustawianie w poszczególnych kontach wtyczkowych musimy poczekać :( ).
* Dodano opcje włączania wtyczki przy starcie AQQ.
* Optymalizacja pobierania tytułu z Last.fm.
* Dodanie nowej opcji w obsłudze - określanie automatyczne odtwarzacza!
* Dodanie ikonki szybkiego włączania/wyłączania wtyczki (domyślnie ikonka jest wyłączona, jej grafika zmienia się w zależności czy uruchomiono czy też nie wtyczkę).

1.0.2.1
-----
* Poprawka ustawiania opisu przy wyładowywaniu wtyczki.
* Poprawka w ucinaniu spacji w pobranym tytule.

1.0.2.0
-----
* Po wyłączeniu odtwarzacza opis zostaje przywrócony i można go zmieniać a co najważniejsze po ponownym włączeniu i wyłączeniu odtwarzacza zostanie przywrócony już opis ten, który ustawiliśmy sobie ostatnio a nie ten przy pierwszym włączeniu wtyczki.
* Poprawiono zapisywanie ustawień Prefix/Suffix ze spacją na końcu (należy ponownie ustawić tekst) oraz wyboru XMPlay i VUPlayer.
* Poprawiono błąd z ustawianiem w opisie 'Winamp (wersja)' przy włączaniu Winampa.
* Podobnie jak wyżej tylko dla foobar2000.
* Zatrzymanie odtwarzania w Winampie, Aimp2 oraz w XMPlay powoduje przywrócenie opisu.
* Podczas wyładowywania wtyczki i wyłączania AQQ przywracany jest poprzedni opis.

1.0.1.0
-----
* Dodanie włączenia okna wtyczki przez ustawienia wtyczek w AQQ.
* Podczas pobierania z AIMP'a już nie ma ciągu znaków '***'.
* Przywracanie opisu po wyłączeniu wtyczki lub braku tekstu z otworem w oknie odtwarzacza.
* Dodanie obsługi VUPlayer i XMPlay

1.0.0.0
-----
* Pierwsza publiczna wersja.