wproducts = A:10 B:10 C:10
wdir = localhost
wport = 8080

cproducts = A:5 B:10 C:3

pproducts = A:3 B:10 C:5

run_warehouse: 
	./bin/warehouse ${wport} ${wproducts}

run_consumer: 
	./bin/consumer ${wdir}:${wport} ${cproducts}

run_producer: 
	./bin/producer ${wdir}:${wport} ${pproducts}

compile:
	gcc src/warehouse.c -o bin/warehouse
	gcc src/consumer.c -o bin/consumer
	gcc src/producer.c -o bin/producer