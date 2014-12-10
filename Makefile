release: release/o3prmeditor

debug: debug/o3prmeditor

clean:
	@echo -n "Cleaning"
	@rm -rf debug
	@rm -rf release
	@echo " : done."
	
release/o3prmeditor:
	[ -d release ] || mkdir release
	cd release;cmake -DCMAKE_BUILD_TYPE=Release ../src
	cd release;make
	

debug/o3prmeditor:
	[ -d debug ] || mkdir debug
	cd debug;cmake -DCMAKE_BUILD_TYPE=Release ../src
	cd debug;make
	