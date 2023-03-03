/*
 * Romset.hpp
 *
 *  Created on: 17.02.2023
 *      Author: nickj
 */

#ifndef SRC_ROMSET_HPP_
#define SRC_ROMSET_HPP_

#include <string>
#include <vector>
#include <unordered_map>
#include <ostream>
#include <list>
#include <functional>
#include <memory>


class Rom {
private:
	friend std::ostream& operator<<(std::ostream &strm, const Rom &rom);
protected:
	std::string m_name, m_filename, m_crc, m_md5;
	int m_size;
public:
	Rom(const char *name = "unnamed", const char *filename = 0, int size = 0,
			const char *crc = 0, const char *md5 = 0);
	virtual std::string md5() {
		return m_md5;
	}
	virtual int size() const {
		return m_size;
	}
	virtual std::string name() const {
		return m_name;
	}
	virtual std::string filename() const {
		return m_filename;
	}

};

//std::ostream& operator<<(std::ostream &strm, const Rom &rom)

class Romset {
private:
	std::string m_name, m_version;
	std::string m_filename;
	std::string m_directory;
	std::string m_url;
	std::unordered_map<std::string, Rom> m_roms;

public:
	Romset(std::string name);
	Romset(std::string filename, std::string directory);
	~Romset();

	const std::string name();
	const std::string version();
	void set_version(std::string version);
	const std::string url();
	void set_url(std::string url);
	std::unordered_map<std::string, Rom> roms();
	bool contains(Rom *rom);
	void import(std::string filepath, Rom *rom);
	std::string filename(){return m_filename;}
	std::string directory(){return m_directory;}
};

template<class... Types>
class Signal {
private:
	std::list<std::function<void(Types ...)>> m_callbacks;
public:
	void connect(std::function<void(Types ...)> callback){
		m_callbacks.push_back(callback);
	}
	/*void connect(void(Types ...) *callback){
		//m_callbacks.push_back(callback);
	}*/
	void emit(Types... args){
		for(auto callback: m_callbacks){
			callback(args... );
		}
	}
};

class RomsetCollection {
private:
	std::list<std::shared_ptr<Romset>> m_romsets;
public:
	Signal<std::shared_ptr<Romset>> added;
	Signal<std::shared_ptr<Romset>> removed;
	void add(std::string name){
		m_romsets.push_back(std::make_shared<Romset>(name));
		//m_romsets.emplace_back(name);
		added.emit(m_romsets.back());
	}
	void add(std::string filename, std::string directory){
		m_romsets.push_back(std::make_shared<Romset>(filename, directory));
		//m_romsets.emplace_back(filename, directory);
		added.emit(m_romsets.back());
	}
	void remove(std::shared_ptr<Romset> rs){
		m_romsets.remove(rs);
	}
	std::list<std::shared_ptr<Romset>>::iterator erase(std::list<std::shared_ptr<Romset>>::iterator rs){
		return m_romsets.erase(rs);
	}
	std::shared_ptr<Romset> find(std::string name){
		for(auto romset : m_romsets){
			if(romset->name() == name)
				return romset;
		}
		return 0;
	}
	auto end(){
		return m_romsets.end();
	}
	auto begin(){
		return m_romsets.begin();
	}
	auto cbegin(){
		return m_romsets.cbegin();
	}
	auto& back(){
		return m_romsets.back();
	}
};

#endif /* SRC_ROMSET_HPP_ */
