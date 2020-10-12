### Šta je OpenGL?

OpenGL se uglavnom smatra za API (**Application Programming Interface**) koji ima na raspolaganju veliki skup funkcija kojima možemo interagovati sa grafikom. Ipak, OpenGL i nije baš API, već specifikacija nastala od strane Khronos Grupe, koja određuje šta treba da bude ulaz/izlaz svake od funkcija, i šta te funkcije treba da rade. Odluka o tačnoj implementaciji je u rukama onoga ko te funkcije implementira, pa se implementacije mogu razlikovati. Proizvođači grafičkih kartica su oni koji prave OpenGL biblioteke, a kompanije (Apple/Microsoft sistemi) ili zajednice (Linux sistemi) su oni koji održavaju biblioteke. U slučaju bagova sa OpenGL-om, uglavnom je dovoljno samo update-ovati drajvere grafičke kartice.



### Core-profile vs Immediate mode

Ranije je programiranje u OpenGL-u značilo programiranje u takozvanom **immediate mode** (takođe poznati i kao **fixed function pipleline**), što je metod koji omogućava dosta jednostavno korišćenje funkcija za iscrtavanje grafike. Većina funkcionalnosti je bilo skrivena unutar biblioteke i programeri nisu imali previše kontrole nad OpenGL izračunavanjima. Immediate mode je dosta lak za korišćenje i razumevanje, ali je jako neefikasan i nefleksibilan. Zato su vremenom funkcije ovog tipa prevaziđene (deprecated) od verzije 3.2 pa naviše, čime je sugerisano da se pređe na **core-profile mode**, što je deo OpenGL specifikacije koji izbacuje sve deprecated funkcionalnosti.

Korišćenjem core-profile mode-a, primorani smo da koristimo moderne principe OpenGL-a, jer ćemo u slučaju korišćenja starih metoda dobiti grešku i iscrtavanje grafike će prestati. Prednosti korišćenja modernog pritupa su fleksibilnost, efikasnost i činjenica da će naše razumevanje funkcionalnosti biti mnogo veće, pa se zbog toga odlučujemo za ovaj pristup, umesto jednostavnijem i neefikasnom immediate mode-a.

Savet je da se ne koristi uvek najnovija verzija OpenGL-a, jer će u tom slučaju samo najnovije grafičke kartice moći da podrže program. Dovoljno je koristiti i verziju 3.3 (prva nakon izbacivanja starih metoda), pa u slučaju potrebe za nekim konkretnim novim stvarima, ići na više verzije.

### Ekstenzije



### Mašina stanja

OpenGL je jedna velika mašina stanja, tj. skup promenljivih koje definišu kako OpenGL treba da se ponaša u datom trenutku. Stanje OpenGL-a je poznato i kao OpenGL **kontekst**. Tokom korišćenja OpenGL-a, menjamo mu stanje postavljanjem određenih opcija, menjanjem bafera i onda renderovanjem korišćenjem trenutnog konteksta.

Svaki put kada hoćemo da na primer iscrtavamo linije umesto trouglova, promenićemo stanje postavljanjem neke promenljive konteksta na opciju crtanja linija. Čim to promenimo, sve naredne operacije iscrtavanja će crtati linije, do sledeće promene ove opcije. 

U zavisnosti od toga kako interaguju sa stanjem, funkcije delimo na dva tipa:

* **funkcije promene stanja** (state-changing functions) - menjanju kontekst
* **funkcije korišćenja stanja** (state-using functions) - izvršavaju određene operacije koristeći trenutne opcije konteksta

### Objekti

OpenGL biblioteke su napisane u C-u. Mnogi konstrukti programskog jezika C nemaju jasno preslikavanje u konstrukte drugih jezika višeg nivoa, pa je zbog toga OpenGL implementiran sa nekim apstrakcijama u vidu. Jedna od tih apstrakcija su OpenGL **objekti**.

Objekat u OpenGL-u je kolekcija opcija koje označavaju podskup opcija stanja. Na primer, možemo imati objekat koji prestavlja podešavanja prozora (visina, širina, ime prozora...).

```c
struct object_name {
    float  option1;
    int    option2;
    char[] name;
};
```

Svi objekti su smešteni u velikom kontekst objektu, što izlgeda otprilike ovako:

```c
// The State of OpenGL
struct OpenGL_Context {
  	...
  	object_name* object_Window_Target;
  	...  	
};
```

Kada hoćemo da koristimo neki objekat, generalno je sledeći princip:

```c
// create object
unsigned int objectId = 0;
glGenObject(1, &objectId);
// bind/assign object to context
glBindObject(GL_WINDOW_TARGET, objectId);
// set options of object currently bound to GL_WINDOW_TARGET
glSetObjectOption(GL_WINDOW_TARGET, GL_OPTION_WINDOW_WIDTH,  800);
glSetObjectOption(GL_WINDOW_TARGET, GL_OPTION_WINDOW_HEIGHT, 600);
// set context target back to default
glBindObject(GL_WINDOW_TARGET, 0);
```

Ovo je workflow koji ćemo često viđati pri radu sa OpenGL-om. Prvo kreiramo objekat i čuvamo referencu ka njemu kao id (pravi podaci su behind the scenes). Onda bindujemo objekat koriscenjem njegovog id-a na ciljnu lokaciju konteksta. Nakon toga podešavamo opcije tog objekta, 







This little piece of code is a workflow you'll frequently see when working with OpenGL. We first create an object and store a reference to it as an id (the real object's data is stored behind the scenes). Then we bind the object (using its id) to the target location of the context (the location of the example window object target is defined as GL_WINDOW_TARGET). Next we set the window options and finally we un-bind the object by setting the current object id of the window target to `0`. The options we set are stored in the object referenced by objectId and restored as soon as we bind the object back to GL_WINDOW_TARGET.

The code samples provided so far are only approximations of how OpenGL operates; throughout the book you will come across enough actual examples.

The great thing about using these objects is that we can define more than one object in our application, set their options and whenever we start an operation that uses OpenGL's state, we bind the object with our preferred settings. There are objects for example that act as container objects for 3D model data (a house or a character) and whenever we want to draw one of them, we bind the object containing the model data that we want to draw (we first created and set options for these objects). Having several objects allows us to specify many models and whenever we want to draw a specific model, we simply bind the corresponding object before drawing without setting all their options again.