use Data::Random;
use XML::Writer;
use IO::File;


my @male_names = ('Jan', 'Andrzej', 'Piotr', 'Krzysztof', 'Stanisław', 'Tomasz', 'Paweł', 'Józef', 'Marcin', 'Marek', 'Michał', 'Grzegorz', 'Jerzy', 'Tadeusz', 'Adam', 'Łukasz', 'Zbigniew', 'Ryszard', 'Dariusz', 'Henryk', 'Mariusz', 'Kazimierz', 'Wojciech', 'Robert', 'Mateusz', 'Marian', 'Rafał', 'Jacek', 'Janusz', 'Mirosław', 'Maciej', 'Sławomir', 'Jarosław', 'Kamil', 'Wiesław', 'Roman', 'Władysław', 'Jakub', 'Artur', 'Zdzisław', 'Edward', 'Mieczysław', 'Damian', 'Dawid', 'Przemysław', 'Sebastian', 'Czesław', 'Leszek', 'Daniel', 'Waldemar');
my @female_names = ('Anna', 'Maria', 'Katarzyna', 'Małgorzata', 'Agnieszka', 'Krystyna', 'Barbara', 'Ewa', 'Elżbieta', 'Zofia', 'Janina', 'Teresa', 'Joanna', 'Magdalena', 'Monika', 'Jadwiga', 'Danuta', 'Irena', 'Halina', 'Helena', 'Beata', 'Aleksandra', 'Marta', 'Dorota', 'Marianna', 'Grażyna', 'Jolanta', 'Stanisława', 'Iwona', 'Karolina', 'Bożena', 'Urszula', 'Justyna', 'Renata', 'Alicja', 'Paulina', 'Sylwia', 'Natalia', 'Wanda', 'Agata', 'Aneta', 'Izabela', 'Ewelina', 'Marzena', 'Wiesława', 'Genowefa', 'Patrycja', 'Kazimiera', 'Edyta', 'Stefania');
my @surnames = ('Nowak', 'Kowalski', 'Wiśniewski', 'Dąbrowski', 'Lewandowski', 'Wójcik', 'Kamiński', 'Kowalczyk', 'Zieliński', 'Szymański', 'Woźniak', 'Kozłowski', 'Jankowski', 'Wojciechowski', 'Kwiatkowski', 'Kaczmarek', 'Mazur', 'Krawczyk', 'Piotrowski', 'Grabowski', 'Nowakowski', 'Pawłowski', 'Michalski', 'Nowicki', 'Adamczyk', 'Dudek', 'Zając', 'Wieczorek', 'Jabłoński', 'Król', 'Majewski', 'Olszewski', 'Jaworski', 'Wróbel', 'Malinowski', 'Pawlak', 'Witkowski', 'Walczak', 'Stępień', 'Górski', 'Rutkowski', 'Michalak', 'Sikora', 'Ostrowski', 'Baran', 'Duda', 'Szewczyk', 'Tomaszewski', 'Pietrzak', 'Marciniak', 'Wróblewski', 'Zalewski', 'Jakubowski', 'Jasiński', 'Zawadzki', 'Sadowski', 'Bąk', 'Chmielewski', 'Włodarczyk', 'Borkowski', 'Czarnecki', 'Sawicki', 'Sokołowski', 'Urbański', 'Kubiak', 'Maciejewski', 'Szczepański', 'Kucharski', 'Wilk', 'Kalinowski', 'Lis', 'Mazurek', 'Wysocki', 'Adamski', 'Kaźmierczak', 'Wasilewski', 'Sobczak', 'Czerwiński', 'Andrzejewski', 'Cieślak', 'Głowacki', 'Zakrzewski', 'Kołodziej', 'Sikorski', 'Krajewski', 'Gajewski', 'Szymczak', 'Szulc', 'Baranowski', 'Laskowski', 'Brzeziński', 'Makowski', 'Ziółkowski', 'Przybylski');

my @cities = ('Gdańsk', 'Warszawa', 'Kraków', 'Olsztyn', 'Szczecin', 'Bydgoszcz', 'Białystok', 'Rzeszów', ' Wrocław', 'Poznań', 'Łódź', 'Słupsk', 'Katowice', ' Sochaczew', 'Ostrołęka');

my @races = ('European', 'Indian', 'Asian', 'American', 'Polynesian', 'African', 'Australian', 'Melanesian');

my @examinations = ('Morfologia', 'Rentgen', 'Badanie moczu', 'Badanie wydolnościowe', 'Rezonans magnetyczny', 'USG', 'EKG', 'inne');

my @chars = ('1'..'9');

my ($number) = @ARGV;
$number = $number ? $number : 0;

print "TRUNCATE TABLE examination;\n";
print "TRUNCATE TABLE patient;\n";

foreach(1..$number){
	my $pesel = '';
	my $tel = '';
	$tel .= @chars[int rand @chars] for (1..11);
	$pesel .= @chars[int rand @chars] for (1..11);
	my $last_name = @surnames[int rand @surnames];
	my $country = 'Polska';
	my $city = @cities[int rand @cities];
	my $race = 'European'; #$races[int rand @races];
	my $sex = '';
	my $name = '';
	my $bdate = Data::Random::rand_date();
	my $edate = Data::Random::rand_date();
	if ($_ % 2 == 0) {
		$sex = 'm';
		$name = @male_names[int rand @male_names];
	} else {
		$sex = 'f';
		$name = @female_names[int rand @female_names];
	}
	my $examination = @examinations[int rand @examinations];
	my $path = '';	

	if (grep {$_ eq $examination} ('Rentgen','Rezonans magnetyczny', 'USG', 'EKG')){
		$path = "png/ex".$_.".png";
		open (my $file, ">", $path) or die $!;
		binmode $file;
		print $file Data::Random::rand_image( width => 256, height => 256, bgcolor => [0,0,0]);
		close($file)
	} else {
		my $result = '';
		$result .= Data::Random::rand_chars(set => 'all', size => 75) foreach(1..4);
		$path = "xml/ex".$_.".xml";
		my $output = IO::File->new(">$path");
		my $writer = XML::Writer->new(OUTPUT => $output);
		$writer->startTag("examination");
		$writer->startTag("patient");
		$writer->startTag("first_name");
		$writer->characters($name);
		$writer->endTag("first_name");
		$writer->startTag("last_name");
		$writer->characters($last_name);
		$writer->endTag("last_name");
		$writer->endTag("patient");
		$writer->startTag("examination_type");
		$writer->characters($examination);
		$writer->endTag("examination_type");
		$writer->startTag("result");
		$writer->characters($result);
		$writer->endTag("result");
		$writer->endTag("examination");
		$writer->end();
		$output->close();
	}
	print "INSERT INTO patient(id, pesel, name, last_name, tel, sex, race, birth_date, birth_city, country) VALUES ($_, $pesel , \'$name\', \'$last_name\', $tel, \'$sex\', \'$race\', \'$bdate\', \'$city\', \'$country\');\n"; 
	$path = "/ex_path/".$path;
	print "INSERT INTO examination(id, patient_id, name, result_path, date) VALUES ($_, $_, \'$examination\', \'$path\', \'$edate\');\n";
}
