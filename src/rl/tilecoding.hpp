/*
 tilecoding.hpp
 Copyright (c) 2023 DEVTAG. Todos os direitos reservados.
  
 Este código faz parte do software SLAMduino, um produto desenvolvido
 pela DEVTAG. Todos os direitos reservados. A reprodução, distribuição,
 modificação ou uso deste software sem a devida autorização por escrito
 da DEVTAG é estritamente proibida.
  
 A DEVTAG não se responsabiliza por qualquer dano ou prejuízo causado
 pelo uso indevido deste software. Utilize-o por sua conta e risco.
  
 Para obter mais informações, entre em contato com a DEVTAG em:
 davi@devtag.com.br
 https://devtag.com.br
 */

#ifndef TILECODING_H
#define TILECODING_H

#include "state.hpp"

#include <math.h>
#include <vector>
#include <random>
#include <map>
#include <string>

#include <fstream>
#include <iostream>

namespace ia {
	namespace rl {
		/// Define a estrutura de um parâmetro livre.
		///
		/// Cada parâmetro livre possui um id e um valor que
		/// indica sua influência na representação de um estado do ambiente.
		class StateFeature {
		public:
			int m_id; ///< Id do parâmetro livre.
			double m_value; ///< Influência do parâmetro livre. Sempre 1 para o algoritmo TileCoding.

			/// Cria um StateFeature.
			StateFeature();

			/// Cria um StateFeature com id e influência.
			/// @param id Id do parâmetro livre.
			/// @param value Influência do parâmetro livre.
			StateFeature(int id, double value);

			/// Cria um StateFeature a partir de outro StateFeature.
            ///
            /// Este construtor cria uma cópia completa de um outro StateFeature.
			StateFeature(const StateFeature &state_feature);

			virtual ~StateFeature() { }

			/// Carrega os parâmetros livres de um LinearFA.
			/// @note Não utilize este operador em StateFeature, ao invés disto,
			/// use em LinearFA.
			/// @see operator>>(std::ostream, LinearFA)
			friend std::istream& operator>>(std::istream& is, StateFeature& sf) {
				// StateFeature
				is.get(); // (
				std::string line;
				getline(is, line, ','); // ,
				sf.m_id = stoi(line);

				getline(is, line, ')'); // )
				sf.m_value = stod(line);

				return is;
			}

			/// Salva os parâmetros livres de um LinearFA.
			/// @note Não utilize este operador em StateFeature, ao invés disto,
			/// use em LinearFA.
			/// @see operator<<(std::ostream, LinearFA)
			friend std::ostream& operator<<(std::ostream& os, const StateFeature& sf) {
				return os << "(" << sf.m_id << "," << sf.m_value << ")";
			}
		};

		/// Define a estrutura de um Tile.
		///
		/// Ao criar um TileCode deve-se configurar as dimensões dos tiles que compõem um tiling.
		/// Todos os tiles de um tiling possuem a mesma dimensão. A sobreposição de vários tilings
		/// acontece com um deslocamento definido na classe TileCode e armazenado em Tiling.
		/// @see ia::rl::Tiling ia::rl::TileCode
		class Tile {
		protected:
			int m_hash_code; ///< Código hash.

		public:
			std::vector<int> m_tiled_vector; ///< Dimensões do tile.

			/// Cria tile sem dimensão e localização.
			Tile();
			
			/// Cria tile de dimensão especificada por **tiled_vector**.
			/// @param tiled_vector Localização deste Tile. Somente as dimensões ativas por **dim_mask** são utilizadas
			/// para determinar sua localização.
			/// @param dim_mask Vetor booleano do mesmo tamanho de **tiled_vector** que indica as
			/// dimensões utilizadas neste Tile.
			/// @see Tiling::get_tile()
			Tile(std::vector<int> &tiled_vector, std::vector<bool> &dim_mask);

            /// Cria um Tile a partir de outro Tile.
            ///
            /// Este construtor cria uma cópia completa de um outro Tile.
			Tile(const Tile &tile);

			~Tile() { }

			/// Utilizado para manter os tiles ordenados.
			///
			/// Verifica se o código hash de um Tile é menor que outro.
			/// @return **True** se o Tile da esquerda é menor que o da direita.
			bool operator <(const Tile& other) const;

			/// Compara se dois Tiles são iguais.
			bool operator ==(const Tile& other) const;

			/// Carrega os parâmetros livres de um LinearFA.
			/// @note Não utilize este operador em Tile, ao invés disto,
			/// use em LinearFA.
			/// @see operator>>(std::ostream, LinearFA)
			friend std::istream& operator>>(std::istream& is, Tile& tile) {
				// Tile
				is.get(); // (
				std::string line;
				getline(is, line, ','); // ,
				tile.m_hash_code = stoi(line);

				// (iii,iii,iii)
				getline(is, line, ')'); // )
				size_t pos = 0;
				std::string token;
				while ((pos = line.find(',')) != std::string::npos) {
					token = line.substr(0, pos);
					tile.m_tiled_vector.push_back(stoi(token));
					line.erase(0, pos+1);
				}
				tile.m_tiled_vector.push_back(stoi(line));
				return is;
			}

			/// Salva os parâmetros livres de um LinearFA.
			/// @note Não utilize este operador em Tile, ao invés disto,
			/// use em LinearFA.
			/// @see operator<<(std::ostream, LinearFA)
			friend std::ostream& operator<<(std::ostream& os, const Tile& tile) {
				os << "(" << tile.m_hash_code << ",";
				for(int i=0;i<tile.m_tiled_vector.size()-1;i++)
					os << tile.m_tiled_vector[i] << ",";		
				return os << tile.m_tiled_vector[tile.m_tiled_vector.size()-1] << ")";
			}
		};

		/// Define a estrutura de um Tiling.
		///
		/// Um Tiling é composto por diversos Tiles que cobrem o espaço de estados do ambiente.
		/// Todos os tiles que compõem um Tiling possuem a mesma dimensão. Para aumentar a resolução,
		/// varios Tiling iguais são sobrepostos com um deslocamento entre si.
		/// @see ia::rl::Tile ia::rl::Tiling ia::rl::TileCoding
		class Tiling {
		private:
			std::vector<double> m_widths; ///< Largura de cada dimensão de um tile.
			/// Offset para sobreposição dos tiling. Esta sobreposição é somada à posição de um Tile.
			/// @see Tiling::get_tile()
			std::vector<double> m_offset;

			std::vector<bool> m_dim_mask; ///< Armazena as dimensões que serão utilizadas para definir um tile.

		public:
			/// Cria um Tiling vazio.
			Tiling();

			/// Cria um Tiling definindo a largura e offset de cada dimensão de um tile.
			/// @param widths Largura de cada dimensão de um tile.
			/// @param offset Offset para sobreposição dos tiling.
			Tiling(std::vector<double> &widths, std::vector<double> &offset);

			/// Cria um Tiling definindo a largura e offset de cada dimensão de um tile com possibilidade de
			/// desativar alguma dimensão dos tiles.
			/// @param widths Largura de cada dimensão de um tile.
			/// @param offset Offset é um vetor do mesmo tamanho de **widths** para definir a sobreposição dos tiling.
			/// @param dim_mask Vetor booleano do mesmo tamanho de **widths** que indica as
			/// dimensões utilizadas neste Tile.
			Tiling(std::vector<double> &widths, std::vector<double> &offset, std::vector<bool> &dim_mask);

            /// Cria um Tiling a partir de outro Tiling.
            ///
            /// Este construtor cria uma cópia completa de um outro Tiling.
			Tiling(const Tiling &tiling);

			virtual ~Tiling() { }

			/// Encontra o tile ativado pela entrada.
			/// @param input Entrada.
			/// @return Tile ativado pela entrada.
			Tile get_tile(std::vector<double> input);

			/// Carrega os parâmetros livres de um LinearFA.
			/// @note Não utilize este operador em FeaturesMap, ao invés disto,
			/// use em LinearFA.
			/// @see operator>>(std::ostream, LinearFA)
			friend std::istream& operator>>(std::istream& is, Tiling& tiling) {
				// Tiling
				is.get(); // (

				// ((ddd,ddd,ddd),(ddd,ddd,ddd),(ddd,ddd,ddd))
				std::string line;

				is.get(); // (
				getline(is, line, ')'); // )
				size_t pos = 0;
				std::string token;
				while ((pos = line.find(',')) != std::string::npos) {
					token = line.substr(0, pos);
					tiling.m_widths.push_back(stod(token));
					line.erase(0, pos+1);
				}
				tiling.m_widths.push_back(stod(line));
				is.get(); // ,
				
				is.get(); // (
				getline(is, line, ')'); // )
				while ((pos = line.find(',')) != std::string::npos) {
					token = line.substr(0, pos);
					tiling.m_offset.push_back(stod(token));
					line.erase(0, pos+1);
				}
				tiling.m_offset.push_back(stod(line));
				is.get(); // ,

				is.get(); // (
				getline(is, line, ')'); // )
				while ((pos = line.find(',')) != std::string::npos) {
					token = line.substr(0, pos);
					tiling.m_dim_mask.push_back(stod(token));
					line.erase(0, pos+1);
				}
				tiling.m_dim_mask.push_back(stod(line));
				is.get(); // )
				return is;
			}

			/// Salva os parâmetros livres de um LinearFA.
			/// @note Não utilize este operador em TileCoding, ao invés disto,
			/// use em LinearFA.
			/// @see operator<<(std::ostream, LinearFA)
			friend std::ostream& operator<<(std::ostream& os, const Tiling& tiling) {
				os << "((";
				for(int i=0;i<tiling.m_widths.size()-1;i++)
					os << tiling.m_widths[i] << ",";		
				os << tiling.m_widths[tiling.m_widths.size()-1] << "),";

				os << "(";
				for(int i=0;i<tiling.m_offset.size()-1;i++)
					os << tiling.m_offset[i] << ",";		
				os << tiling.m_offset[tiling.m_offset.size()-1] << "),";

				os << "(";
				for(int i=0;i<tiling.m_dim_mask.size()-1;i++)
					os << tiling.m_dim_mask[i] << ",";		
				return os << tiling.m_dim_mask[tiling.m_dim_mask.size()-1] << "))";
			}
		};

		/// Aproximador de função TileCoding.
		///
		/// Um aproximador de função TileCoding é formador por diversos tilings sobrepostos com um
		/// deslocamento entre si. Cada tiling é formado por diversos tiles que mapeiam o espaço de
		/// estados do ambiente.
		/// @see ia::rl::Tiling ia::rl::Tile
		/// ### Exemplo
		/// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~.cpp
		/// ia::rl::TileCoding tc(); // Cria parâmetros livres do aproximador de funções
		/// std::vector<double> widths { 10, 10 }; // Dimensão dos tiles
		/// tc.add_tiling(std::vector<bool> { true, true }, widths, 5); // Adiciona 5 tilings com 2 dimensões
		/// tc.add_tiling(std::vector<bool> { false, true }, widths, 3); // Adiciona 3 tilings com 1 dimensão
		///
		/// std::vector<ia::rl::StateFeatures> features = tc.features(...); // Obtém os tiles ativos por um estado
		/// for (ia::rl::StateFeatures sf : features)
		///		cout << sf.m_id << " "; // Mostra número id dos tiles
		/// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
		class TileCoding {
		private:
			std::default_random_engine m_gen; ///< Gerador de números aleatórios.
			std::uniform_real_distribution<float> m_distribution; ///< Distribuição uniforme entre 0 e 1.

			/// Mantém um id único para cada parâmetro livre do modelo.
			/// @see TileCoding::get_or_gen_feature()
			int m_feature_id;

			std::vector<Tiling> m_tilings; ///< Tilings do modelo.
			std::vector<std::map<Tile, int>> m_state_features; ///< Mapeamento dos tiles em um número id.

			/// Cria ou recupera o id de um Tile dentro de um Tiling.
			/// @param tile_map Tiles de um Tiling.
			/// @param tile Tile que deseja obter seu id.
			/// @return Id do tile.
			int get_or_gen_feature(std::map<Tile, int> &tile_map, Tile &tile);

			/// Randomiza o deslocamento dos tilings.
			/// @param dim_mask booleano do mesmo tamanho de **widths** que indica as
			/// dimensões utilizadas neste Tile.
			/// @param widths Largura de cada dimensão de um tile.
			/// @return Vetor de números reais aleatórios para cada dimensão especificada em **dim_mask**.
			std::vector<double> rand_offset(std::vector<bool> &dim_mask, std::vector<double> &widths);

		public:
			/// Cria um TileCoding vazio.
			///
			/// Adicione tilings com a função add_tiling().
			TileCoding();

            /// Cria um TileCoding a partir de outro TileCoding.
            ///
            /// Este construtor cria uma cópia completa de um outro TileCoding.
			TileCoding(const TileCoding &tile_coding);

			virtual ~TileCoding() { }

			/// Adiciona um conjunto de tilings sobrepostos.
			/// @param dim_mask booleano do mesmo tamanho de **widths** que indica as
			/// dimensões utilizadas neste Tile.
			/// @param widths Largura de cada dimensão de um tile.
			/// @param num_tilings Números de tilings sobrepostos.
			void add_tiling(std::vector<bool> &dim_mask, std::vector<double> &widths, int num_tilings);

			/// Obtém todos os parâmetros livres relacionados a um estado **s**.
			/// @param s Estado **s** que se deseja obter os parâmetros livres.
			/// @return Vetor com conjunto de parametros livres associados ao estado **s**
			std::vector<StateFeature> features(ia::rl::State *s);

			/// Obtém a quantidade de parametros livres utilizados até o momento.
			/// @return Quantidade de parâmetros livres utilizados.
			int num_features();

			/// Carrega os parâmetros livres de um LinearFA.
			/// @note Não utilize este operador em TileCoding, ao invés disto,
			/// use em LinearFA.
			/// @see operator>>(std::ostream, LinearFA)
			friend std::istream& operator>>(std::istream& is, TileCoding& tilecode) {
				// TileCoding
				is.get(); // (
				is.get(); // (
				std::string line;
				getline(is, line, ','); // ,
				tilecode.m_feature_id = stoi(line);

				// ((iii,...,...),(((...,iii),(...,iii)),((...,iii),(...,iii))))
				char c;
				do {
					Tiling tiling;
					is >> tiling; // ...
					c = is.get(); // , or )
					tilecode.m_tilings.push_back(tiling);
				} while(c != ')');
				
				is.get(); // ,
				is.get(); // (
				do {
					is.get(); // (
					std::map<Tile, int> state_features;
					do {
						Tile key;
						int value;

						is.get(); // (
						is >> key; // ...
						is.get(); // ,
						getline(is, line, ')'); // )
						value = stoi(line);
						state_features[key] = value;

						c = is.get(); // , or )
					}while(c != ')');
					tilecode.m_state_features.push_back(state_features);
					c = is.get(); // , or )
				} while(c != ')');
				is.get(); // )
				return is;
			}

			/// Salva os parâmetros livres de um LinearFA.
			/// @note Não utilize este operador em TileCoding, ao invés disto,
			/// use em LinearFA.
			/// @see operator<<(std::ostream, LinearFA)
			friend std::ostream& operator<<(std::ostream& os, const TileCoding& tilecode) {
				os << "((" << tilecode.m_feature_id << ",";
				for(int i=0;i<tilecode.m_tilings.size()-1;i++)
					os << tilecode.m_tilings[i] << ",";		
				os << tilecode.m_tilings[tilecode.m_tilings.size()-1] << ")";

				os << ",(";
				for(int i=0;i<tilecode.m_state_features.size()-1;i++) {
					auto it = tilecode.m_state_features[i].begin();
					auto it2 = --tilecode.m_state_features[i].end();
					os << "(";
					for(; it != it2; ++it)
						os << "(" << it->first << "," << it->second << "),";
					os << "(" << it->first << "," << it->second << ")";
					os << "),";
				}

				auto it = tilecode.m_state_features[tilecode.m_state_features.size()-1].begin();
				auto it2 = --tilecode.m_state_features[tilecode.m_state_features.size()-1].end();
				os << "(";
				for(; it != it2; ++it)
					os << "(" << it->first << "," << it->second << "),";
				os << "(" << it->first << "," << it->second << "))";
				return os << "))";
			}
		};
	}
}

#endif