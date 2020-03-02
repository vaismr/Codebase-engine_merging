#pragma once

namespace NCL {
	namespace CSC8503 {
		class State		{
		public:
			State() {}
			virtual ~State() {}
			virtual void Update() = 0; //Pure virtual base class
		};

		typedef void(*StateFunc)(void*);
		typedef void(*EnemyFunc)(void*, void*);

		class GenericState : public State		{
		public:
			GenericState(StateFunc someFunc, void* someData) {
				func		= someFunc;
				funcData	= someData;
			}
			virtual void Update() {
				if (funcData != nullptr) {
					func(funcData);
				}
			}
		protected:
			StateFunc func;
			void* funcData;
		};

		class EnemyState : public State {
		public:
			EnemyState(EnemyFunc enemyFunc, void* enemy, void* player = nullptr) {
				func = enemyFunc;
				this->enemy = enemy;
				this->player = player;
			}

			virtual void Update() {
				if (enemy != nullptr && player != nullptr)
					func(enemy, player);
			}
		protected:
			EnemyFunc func;
			void* enemy;
			void* player;
		};
	}
}