#include <stdio.h>
#include <okzarena.h>

// Game data structures
typedef struct {
    int id;
    char name[32];
    int damage;
    float weight;
} Weapon;

typedef struct {
    int id;
    char name[32];
    int defense;
    float durability;
} Armor;

typedef struct {
    int health;
    int mana;
    int experience;
    char class_name[16];
} PlayerStats;

typedef struct {
    int gold;
    int item_count;
    char inventory_items[10][32];  // Simple item names
} GameState;

void test_basic_operations() {
    printf("=== Testing Basic Operations ===\n");
 
    int level = 42;
    float health = 85.5f;
    char player_name[] = "DragonSlayer";

    okz_push("player_level", level);
    okz_push("player_health", health);
    okz_push("player_name", player_name);

    // Retrieve and verify
    int* stored_level = okz_get(int, "player_level");
    float* stored_health = okz_get(float, "player_health");
    char* stored_name = okz_get(char, "player_name");

    printf("Level: %d (expected: 42)\n", *stored_level);
    printf("Health: %.1f (expected: 85.5)\n", *stored_health);
    printf("Name: %s (expected: DragonSlayer)\n", stored_name);

    debug_arena();
}

void test_complex_structs() {
  printf("\n=== Testing Complex Structures ===\n");

  Weapon sword = { 1, "Excalibur", 50, 3.5f };
  Weapon bow = { 2, "Bow", 25, 1.2f };

  Armor helmet = {10, "Dragon Helm", 25, 100.0f};

  okz_push("weapon_sword", sword);
  okz_push("weapon_bow", bow);
  okz_push("armor_helmet", helmet);

  Weapon* stored_sword = okz_get(Weapon, "weapon_sword");
  Weapon* stored_bow = okz_get(Weapon, "weapon_bow");
  Armor* stored_helmet = okz_get(Armor, "armor_helmet");

  printf("Sword: %s, damage: %d, weight: %.1f\n", 
           stored_sword->name, stored_sword->damage, stored_sword->weight);
  printf("Bow: %s, damage: %d, weight: %.1f\n", 
          stored_bow->name, stored_bow->damage, stored_bow->weight);
  printf("Helmet: %s, defense: %d, durability: %.1f\n", 
          stored_helmet->name, stored_helmet->defense, stored_helmet->durability);
  
  debug_arena();
}

void test_arrays_and_bulk_data() {
    printf("\n=== Testing Arrays and Bulk Data ===\n");

    int treasure_values[] = {100, 250, 500, 1000, 2500};
    okz_push("treasure_values", treasure_values);

    GameState game_state = {
        .gold = 1337,
        .item_count = 3,
        .inventory_items = {"Health Potion", "Mana Crystal", "Ancient Scroll"}
    };
    okz_push("game_state", game_state);

    // Retrieve array
    int* treasures = okz_get(int, "treasure_values");
    printf("Treasure values: ");
    for (int i = 0; i < 5; i++) {
        printf("%d ", treasures[i]);
    }
    printf("\n");

    // Retrieve game state
    GameState* state = okz_get(GameState, "game_state");
    printf("Gold: %d, Items: %d\n", state->gold, state->item_count);
    printf("Inventory: ");
    for (int i = 0; i < state->item_count; i++) {
        printf("%s ", state->inventory_items[i]);
    }
    printf("\n");

    debug_arena();
}

void test_memory_management() {
  printf("\n=== Testing Memory Management ===\n");

  for (int i = 0; i < 10; ++i) {
    char key[32];
    snprintf(key, sizeof(key), "item_%d", i);

    Weapon temp_weapon = {i, "Generic Sword", 10 + i, 1.0f + i * 0.1f};
    okz_push(key, temp_weapon);
  }

  printf("Added 10 weapons:\n");
  debug_arena();

  okz_release("item_3");
  okz_release("item_7");
  okz_release("item_1");

  printf("After removing items 1, 3, 7:\n");
  debug_arena();

  Weapon* removed = okz_get(Weapon, "item_3");
  if (removed == NULL) {
    printf("Correctly returned NULL for removed item\n");
  } else {
    printf("ERROR: Retrieved removed item!\n");
  }

  Weapon* remaining = okz_get(Weapon, "item_5");
  if (remaining != NULL) {
    printf("Successfully retrieved remaining item: %s\n", remaining->name);
  } else {
    printf("ERROR: Could not retrieve existing item!\n");
  }
}

void test_edge_cases() {
  printf("\n=== Testing Edge Cases ===\n");

  // Test overwriting
  int original_value = 100;
  int new_value = 200;

  okz_push("test_key", original_value);
  int* retrieved = okz_get(int, "test_key");
  printf("Original value: %d\n", *retrieved);
  *retrieved = new_value;

  retrieved = okz_get(int, "test_key");
  printf("After 'overwrite': %d (Note: this creates duplicate keys)\n", *retrieved);

  debug_arena();

  // Test empty key (should fail)
  int test_val = 42;
  int result = okz_push_copy("", &test_val, sizeof(test_val));
  if (result != 0) {
      printf("Correctly rejected empty key\n");
  }

  // Test NULL data
  result = okz_push_copy("null_test", NULL, 0);
  if (result != 0) {
      printf("Correctly handled NULL data with size 0\n");
  }
}


int main(void) {
  okz_init();

  test_basic_operations();
  test_complex_structs();
  test_arrays_and_bulk_data();
  test_memory_management();
  test_edge_cases();

  okz_destroy();
  return 0;
}
