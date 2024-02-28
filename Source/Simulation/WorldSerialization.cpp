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

std::string SignalKindToString(SignalKind Kind)
{
	switch (Kind)
	{
	case SignalKind::Manual:
		return "manual";
	case SignalKind::Automatic:
		return "automatic";
	default:
		BD_ASSERT(false);
	}
}

json SerializeSignal(const Signal& Signal)
{
	json Result;

	Result["from"] = std::array{ Signal.Location.FromTile.x, Signal.Location.FromTile.y };
	Result["to"] = std::array{ Signal.Location.ToTile.x, Signal.Location.ToTile.y };
	Result["state"] = Signal.State;
	Result["kind"] = SignalKindToString(Signal.Kind);

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

json SerializeTrackArea(const TrackArea& TrackArea)
{
	json Result;

	Result["name"] = TrackArea.Name;
	Result["entry_points"] = json::array();
	for (const auto& EntryPoint : TrackArea.EntryPoints)
	{
		json JSONEntryPoint = {
			{ "from", json::array({ EntryPoint.TileFrom.x, EntryPoint.TileFrom.y }) },
			{ "to", json::array({ EntryPoint.TileTo.x, EntryPoint.TileTo.y }) },
		};
		Result["entry_points"].push_back(JSONEntryPoint);
	}
	Result["stopping_points"] = json::array();
	for (const auto& StoppingPoint : TrackArea.StoppingPoints)
	{
		json JSONStoppingPoint = {
			{ "from", json::array({ StoppingPoint.TileFrom.x, StoppingPoint.TileFrom.y }) },
			{ "to", json::array({ StoppingPoint.TileTo.x, StoppingPoint.TileTo.y }) },
		};
		Result["entry_points"].push_back(JSONStoppingPoint);
	}

	return Result;
}

json SerializeTrackAreas(const World& World)
{
	json Result;

	size_t Index = 0;
	for (const auto& TrackArea : World.TrackAreas())
	{
		Result[Index++] = SerializeTrackArea(TrackArea);
	}

	return Result;
}

json SerializeExit(const Exit& Exit)
{
	json Result = {
		{ "name", Exit.Name },
		{ "location", json::array({ Exit.Location.x, Exit.Location.y }) },
		{ "spawn_direction", TrackDirectionToString(Exit.SpawnDirection) }
	};
	return Result;
}

json SerializeExits(const World& World)
{
	json Result;

	for (const auto& Exit : World.Exits())
		Result.push_back(SerializeExit(Exit));

	return Result;
}

json SerializeTimetable(const Timetable& Timetable)
{
	json Result = {
		{ "spawn_time", Timetable.SpawnTime.SecondsSinceStart() },
		{ "arrival_time", Timetable.ArrivalTime.SecondsSinceStart() },
		{ "departure_time", Timetable.DepartureTime.SecondsSinceStart() },
		{ "leave_time", Timetable.LeaveTime.SecondsSinceStart() },
		{ "spawn_location", Timetable.SpawnLocation },
		{ "preferred_track", Timetable.PreferredTrack },
		{ "leave_location", Timetable.LeaveLocation },
		{ "min_stop", Timetable.MinStopDuration },
	};
	return Result;
}

json SerializeTrain(const Train& Train)
{
	json Result;

	Result["id"] = Train.ID;
	Result["direction"] = Train.Direction;
	Result["offset"] = Train.OffsetInTile;
	Result["tile"] = std::array{ Train.Tile.x, Train.Tile.y };
	Result["length"] = Train.Length;
	Result["timetable"] = SerializeTimetable(Train.Timetable);

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
	Root["track_areas"] = SerializeTrackAreas(World);
	Root["exits"] = SerializeExits(World);
	Root["trains"] = SerializeTrains(World);

	return Root.dump(4);
}

/*
 * Parses a JSON object of the form:
 * {
 *     "from": [ FromX, FromY ],
 *	   "to": [ ToX, ToY ],
 * }
 * into a pair of vectors { { FromX, FromY }, { ToX, ToY } }
 */
std::optional<std::pair<glm::ivec2, glm::ivec2>> DeserializeFromToVecPair(const json& JSON)
{
	if (!JSON.contains("from") || !JSON["from"].is_array() || JSON["from"].size() != 2 ||
		!JSON["from"][0].is_number_integer() || !JSON["from"][1].is_number_integer() ||
		!JSON.contains("to") || !JSON["to"].is_array() || JSON["to"].size() != 2 ||
		!JSON["to"][0].is_number_integer() || !JSON["to"][1].is_number_integer())
	{
		return std::nullopt;
	}

	return std::make_pair(
		glm::ivec2(JSON["from"][0], JSON["from"][1]),
		glm::ivec2(JSON["to"][0], JSON["to"][1])
	);
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

SignalKind SignalKindFromString(std::string_view SignalKind)
{
	if (SignalKind == "manual")
		return SignalKind::Manual;
	if (SignalKind == "automatic")
		return SignalKind::Automatic;
	BD_UNREACHABLE();
}

std::optional<Signal> DeserializeSignal(const json& Signal)
{
	if (!Signal.contains("state") || !Signal["state"].is_number_unsigned())
	{
		BD_LOG_WARNING("Invalid signal description");
		return std::nullopt;
	}

	auto MaybeFromToPair = DeserializeFromToVecPair(Signal);
	if (!MaybeFromToPair.has_value())
		return std::nullopt;
	auto [From, To] = MaybeFromToPair.value();

	struct Signal Result = {
		.Location = {
			.FromTile = From,
			.ToTile = To
		},
		.State = Signal["state"],
		.Kind = SignalKindFromString(Signal["kind"])
	};
	return Result;
}

std::optional<TrackArea> DeserializeTrackArea(const json& JSONTrackArea)
{
	if (!JSONTrackArea.contains("name") || !JSONTrackArea["name"].is_string() ||
		!JSONTrackArea.contains("entry_points") || !JSONTrackArea["entry_points"].is_array() ||
		!JSONTrackArea.contains("stopping_points") || !JSONTrackArea["stopping_points"].is_array())
	{
		return std::nullopt;
	}

	std::vector<TrackAreaLocation> EntryPoints;
	for (const auto& EntryPoint : JSONTrackArea["entry_points"])
	{
		auto MaybeFromToVecPair = DeserializeFromToVecPair(EntryPoint);
		if (!MaybeFromToVecPair.has_value())
			return std::nullopt;
		auto [From, To] = MaybeFromToVecPair.value();
		EntryPoints.emplace_back(From, To);
	}

	std::vector<TrackAreaLocation> StoppingPoints;
	for (const auto& StoppingPoint : JSONTrackArea["stopping_points"])
	{
		auto MaybeFromToVecPair = DeserializeFromToVecPair(StoppingPoint);
		if (!MaybeFromToVecPair.has_value())
			return std::nullopt;
		auto [From, To] = MaybeFromToVecPair.value();
		StoppingPoints.emplace_back(From, To);
	}

	return TrackArea{
		.Name = JSONTrackArea["name"],
		.EntryPoints = EntryPoints,
		.StoppingPoints = StoppingPoints
	};
}

std::optional<Exit> DeserializeExit(const json& JSONExit)
{
	if (!JSONExit.contains("name") || !JSONExit["name"].is_string() ||
		!JSONExit.contains("location") || !JSONExit["location"].is_array() ||
		JSONExit["location"].size() != 2 ||
		!JSONExit["location"][0].is_number_integer() || !JSONExit["location"][1].is_number_integer() ||
		!JSONExit.contains("spawn_direction") || !JSONExit["spawn_direction"].is_string())
	{
		return std::nullopt;
	}

	return Exit{
		.Name = JSONExit["name"],
		.Location = { JSONExit["location"][0], JSONExit["location"][1] },
		.SpawnDirection = TrackDirectionFromString(JSONExit["spawn_direction"])
	};
}

std::optional<Timetable> DeserializeTimetable(const json& JSONTimetable)
{
	if (!JSONTimetable.contains("spawn_time") || !JSONTimetable["spawn_time"].is_number() ||
		!JSONTimetable.contains("arrival_time") || !JSONTimetable["arrival_time"].is_number() ||
		!JSONTimetable.contains("departure_time") || !JSONTimetable["departure_time"].is_number() ||
		!JSONTimetable.contains("leave_time") || !JSONTimetable["leave_time"].is_number() ||
		!JSONTimetable.contains("spawn_location") || !JSONTimetable["spawn_location"].is_string() ||
		!JSONTimetable.contains("preferred_track") || !JSONTimetable["preferred_track"].is_string() ||
		!JSONTimetable.contains("leave_location") || !JSONTimetable["leave_location"].is_string() ||
		!JSONTimetable.contains("min_stop") || !JSONTimetable["min_stop"].is_number())
	{
		return std::nullopt;
	}

	auto Result = Timetable(
		WorldTime::FromSeconds(JSONTimetable["spawn_time"]),
		WorldTime::FromSeconds(JSONTimetable["arrival_time"]),
		WorldTime::FromSeconds(JSONTimetable["departure_time"]),
		WorldTime::FromSeconds(JSONTimetable["leave_time"]),
		JSONTimetable["spawn_location"],
		JSONTimetable["preferred_track"],
		JSONTimetable["leave_location"],
		JSONTimetable["min_stop"]
	);
	return Result;
}

std::optional<Train> DeserializeTrain(const json& Train)
{
	if (!Train.contains("id") || !Train["id"].is_string() ||
		!Train.contains("direction") || !Train["direction"].is_number_unsigned() ||
		!Train.contains("offset") || !Train["offset"].is_number() ||
		!Train.contains("tile") || !Train["tile"].is_array() || Train["tile"].size() != 2 ||
		!Train["tile"][0].is_number() || !Train["tile"][1].is_number() ||
		!Train.contains("length") || !Train["length"].is_number() ||
		!Train.contains("timetable") || !Train["timetable"].is_object())
	{
		return std::nullopt;
	}

	auto Timetable = DeserializeTimetable(Train["timetable"]);
	if (!Timetable.has_value())
		return std::nullopt;

	struct Train Result = {
		.ID = Train["id"],
		.Tile = { Train["tile"][0], Train["tile"][1] },
		.OffsetInTile = Train["offset"],
		.Direction = Train["direction"],
		.Length = Train["length"],
		.Timetable = Timetable.value(),
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

	if (Root.contains("track_areas"))
	{
		for (const auto& TrackArea : Root["track_areas"])
		{
			auto NewTrackArea = DeserializeTrackArea(TrackArea);
			if (NewTrackArea.has_value())
				Result.AddTrackArea(NewTrackArea.value());
		}
	}

	if (Root.contains("exits"))
	{
		for (const auto& Exit : Root["exits"])
		{
			auto NewExit = DeserializeExit(Exit);
			if (NewExit.has_value())
				Result.AddExit(NewExit.value());
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
