#include "WorldSerialization.h"

#include <nlohmann/json.hpp>
#include <ranges>

using namespace nlohmann;

json SerializeMeta(const World& World)
{
	json Result;

	Result["time"] = World.CurrentTime().SecondsSinceStart();

	return Result;
}

json SerializeTile(const TrackTile& Tile)
{
	json Result;

	Result["coordinates"] = std::array{ Tile.Tile.x, Tile.Tile.y };
	Result["directions"] = Tile.ConnectedDirections;
	Result["selected_path"] = Tile.SelectedPath;

	Result["states"] = json::array();
	ForEachExistingDirection(Tile.ConnectedDirections, [&](TrackDirection Direction)
	{
		Result["states"].push_back(Tile.State(Direction));
	});

	return Result;
}

json SerializeTiles(const World& World)
{
	json Result;

	size_t Index = 0;
	for (const auto& Tile : World.TrackTiles())
	{
		Result[Index] = SerializeTile(Tile);
		++Index;
	}

	return Result;
}

json SerializeSignal(const Signal& Signal)
{
	json Result;

	Result["from"] = std::array{ Signal.Location.FromTile.x, Signal.Location.FromTile.y };
	Result["to"] = std::array{ Signal.Location.ToTile.x, Signal.Location.ToTile.y };
	Result["state"] = Signal.State;

	return Result;
}

json SerializeSignals(const World& World)
{
	json Result;

	size_t Index = 0;
	for (const auto& Signal : World.Signals())
	{
		Result[Index++] = SerializeSignal(Signal);
	}

	return Result;
}

json SerializeTrain(const Train& Train)
{
	json Result;

	Result["direction"] = Train.Direction;
	Result["offset"] = Train.OffsetInTile;
	Result["tile"] = std::array{ Train.Tile.x, Train.Tile.y };
	Result["length"] = Train.Length;

	return Result;
}

json SerializeTrains(const World& World)
{
	json Result;

	size_t Index = 0;
	for (const auto& Train : World.Trains())
	{
		Result[Index++] = SerializeTrain(Train);
	}

	return Result;
}

std::string WorldSerialization::Serialize(const World& World)
{
	json Root;

	Root["meta"] = SerializeMeta(World);
	Root["tiles"] = SerializeTiles(World);
	Root["signals"] = SerializeSignals(World);
	Root["trains"] = SerializeTrains(World);

	return Root.dump(4);
}

std::optional<TrackTile> DeserializeTile(const json& Tile)
{
	if (!Tile.contains("coordinates") || !Tile["coordinates"].is_array() || Tile["coordinates"].size() != 2 ||
		!Tile["coordinates"][0].is_number_integer() || !Tile["coordinates"][1].is_number_integer() ||
		!Tile.contains("directions") || !Tile["directions"].is_number_unsigned() ||
		!Tile.contains("selected_path") || !Tile["selected_path"].is_number_unsigned() ||
		!Tile.contains("states") || !Tile["states"].is_array())
	{
		BD_LOG_WARNING("Invalid tile description");
		return std::nullopt;
	}

	TrackTile Result(glm::ivec2(Tile["coordinates"][0], Tile["coordinates"][1]), Tile["directions"]);
	Result.SelectedPath = Tile["selected_path"];

	size_t NextStateIndex = 0;
	ForEachExistingDirection(Result.ConnectedDirections, [&](TrackDirection Direction)
	{
		Result.SetState(Direction, Tile["states"][NextStateIndex++]);
	});

	return Result;
}

std::optional<Signal> DeserializeSignal(const json& Signal)
{
	if (!Signal.contains("from") || !Signal["from"].is_array() || Signal["from"].size() != 2 ||
		!Signal["from"][0].is_number_integer() || !Signal["from"][1].is_number_integer() ||
		!Signal.contains("to") || !Signal["to"].is_array() || Signal["to"].size() != 2 ||
		!Signal["to"][0].is_number_integer() || !Signal["to"][1].is_number_integer() ||
		!Signal.contains("state") || !Signal["state"].is_number_unsigned())
	{
		BD_LOG_WARNING("Invalid signal description");
		return std::nullopt;
	}

	struct Signal Result = {
		.Location = {
			.FromTile = { Signal["from"][0], Signal["from"][1] },
			.ToTile = { Signal["to"][0], Signal["to"][1] }
		},
		.State = Signal["state"]
	};
	return Result;
}

std::optional<Train> DeserializeTrain(const json& Train)
{
	if (!Train.contains("direction") || !Train["direction"].is_number_unsigned() ||
		!Train.contains("offset") || !Train["offset"].is_number() ||
		!Train.contains("tile") || !Train["tile"].is_array() || Train["tile"].size() != 2 ||
		!Train["tile"][0].is_number() || !Train["tile"][1].is_number() ||
		!Train.contains("length") || !Train["length"].is_number())
	{
		return std::nullopt;
	}

	struct Train Result = {
		.Tile = { Train["tile"][0], Train["tile"][1] },
		.OffsetInTile = Train["offset"],
		.Direction = Train["direction"],
		.Length = Train["length"],
		// FIXME: .Timetable = 
	};
	return Result;
}

World WorldSerialization::Deserialize(std::string_view Source)
{
	World Result;

	json Root = json::parse(Source);

	if (!Root.contains("meta") || !Root["meta"].contains("time"))
	{
		BD_LOG_WARNING("Cannot deserialize world with invalid metadata");
		return Result;
	}
	Result.OverrideTime(WorldTime::FromSeconds(Root["meta"]["time"]));

	if (Root.contains("tiles"))
	{
		for (const auto& Tile : Root["tiles"])
		{
			auto NewTile = DeserializeTile(Tile);
			if (NewTile.has_value())
				Result.OverwriteTile(NewTile.value());
		}
	}

	if (Root.contains("signals"))
	{
		for (const auto& Signal : Root["signals"])
		{
			auto NewSignal = DeserializeSignal(Signal);
			if (NewSignal.has_value())
				Result.OverwriteSignal(NewSignal.value());
		}
	}

	if (Root.contains("trains"))
	{
		for (const auto& Train : Root["trains"])
		{
			auto NewTrain = DeserializeTrain(Train);
			if (NewTrain.has_value())
				Result.AddTrainUnsafe(NewTrain.value()); // FIXME: make it safe :)
		}
	}

	return Result;
}
