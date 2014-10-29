# TuneStatus
TuneStatus jest wtyczką do komunikatora [AQQ](http://www.aqq.eu/pl.php). Służy do informowania naszych znajomych o tym co aktualnie słuchamy w odtwarzaczu plików audio. Informowanie odbywa się poprzez zmianę naszego opisu oraz opcjonalnie poprzez wysyłanie notyfikacji User Tune (XEP-0118) w sieci Jabber.

### Wymagania
Do skompilowania wtyczki potrzebne jest:

* Embarcadero RAD Studio XE7 (C++Builder oraz Delphi, jeżeli chcemy kompilować wersję x64)
* Pełna wersja komponentów [AlphaControls](http://www.alphaskins.com/) (do kompilacji wersji x64 potrzebne jest Delphi)
* [Plik nagłówkowy SDK komunikatora AQQ](https://bitbucket.org/beherit/pluginapi-for-aqq-im)
* Opcjonalnie [UPX](http://upx.sourceforge.net/) dla zmniejszenia rozmiaru pliku wynikowego (w szczególności wersji x64)

### Błędy
Znalezione błędy wtyczki należy zgłaszać na [tracekerze](http://forum.aqq.eu/tracker/project-25-tunestatus/) znajdującym na oficjalnym forum komunikatora AQQ lub pisząc bezpośrednio do autora wtyczki (preferowany kontakt poprzez Jabber).

### Kontakt z autorem
Autorem wtyczki TuneStatus jest Krzysztof Grochocki. Możesz skontaktować się z nim drogą mailową (kontakt@beherit.pl) lub poprzez Jabber (im@beherit.pl).

### Licencja
Wtyczka TuneStatus objęta jest licencją [GNU General Public License 3](http://www.gnu.org/copyleft/gpl.html).

~~~~
TuneStatus
Copyright (C) 2009-2014  Krzysztof Grochocki

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.
~~~~