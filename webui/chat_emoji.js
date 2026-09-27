// Copyright (c) 2026 CIYAM Developers
//
// Distributed under the MIT/X11 software license, please refer to the file license.txt
// in the root project directory or http://www.opensource.org/licenses/mit-license.php.

// NOTE: The emoji offered by the chat's emoji panel - a curated set, not all of Unicode. Each
// is written as its code points, so this file stays plain ASCII whatever it is served as, and
// is drawn by the device's own emoji font: no images and no outside requests.
//
// Left out on purpose:
//
//   - country flags - Windows draws them as two letters ("AU"), not as a flag
//   - skin tone and family sequences - many, long in bytes, and uneven between systems
//   - anything newer than Emoji 12 (2019), which older systems show as an empty box
//
// Each entry is "<code points>|<name>|<more words to search by>".

const c_emoji_categories = [
{
   id: "smileys", label: "Smileys", icon: "1F600",
   items: [
      "1F600|grinning face|smile happy", "1F603|grinning face with big eyes|smile happy",
      "1F604|grinning face with smiling eyes|smile happy", "1F601|beaming face|grin",
      "1F606|grinning squinting face|laugh", "1F605|grinning face with sweat|relief phew",
      "1F923|rolling on the floor laughing|rofl lol", "1F602|face with tears of joy|lol laugh",
      "1F642|slightly smiling face|smile", "1F643|upside-down face|silly",
      "1F609|winking face|wink", "1F60A|smiling face with smiling eyes|blush happy",
      "1F607|smiling face with halo|angel innocent", "1F970|smiling face with hearts|love adore",
      "1F60D|smiling face with heart-eyes|love crush", "1F929|star-struck|wow amazing",
      "1F618|face blowing a kiss|kiss", "1F617|kissing face|kiss",
      "263A FE0F|smiling face|smile", "1F61A|kissing face with closed eyes|kiss",
      "1F619|kissing face with smiling eyes|kiss", "1F60B|face savoring food|yum tasty",
      "1F61B|face with tongue|tongue", "1F61C|winking face with tongue|joke kidding",
      "1F92A|zany face|crazy wild", "1F61D|squinting face with tongue|tongue",
      "1F911|money-mouth face|rich money", "1F917|hugging face|hug",
      "1F92D|face with hand over mouth|oops giggle", "1F92B|shushing face|quiet secret shh",
      "1F914|thinking face|hmm think", "1F910|zipper-mouth face|secret quiet",
      "1F928|face with raised eyebrow|suspicious doubt", "1F610|neutral face|meh",
      "1F611|expressionless face|blank", "1F636|face without mouth|speechless",
      "1F60F|smirking face|smirk", "1F612|unamused face|meh",
      "1F644|face with rolling eyes|eyeroll whatever", "1F62C|grimacing face|awkward",
      "1F925|lying face|lie", "1F60C|relieved face|relief calm",
      "1F614|pensive face|sad", "1F62A|sleepy face|tired",
      "1F924|drooling face|drool", "1F634|sleeping face|zzz sleep",
      "1F637|face with medical mask|sick mask", "1F912|face with thermometer|ill sick",
      "1F915|face with head-bandage|hurt", "1F922|nauseated face|sick",
      "1F92E|face vomiting|sick", "1F927|sneezing face|sick",
      "1F975|hot face|heat", "1F976|cold face|freezing",
      "1F974|woozy face|dizzy", "1F635|dizzy face|dizzy",
      "1F92F|exploding head|mind blown", "1F920|cowboy hat face|cowboy",
      "1F973|partying face|party celebrate", "1F60E|smiling face with sunglasses|cool",
      "1F913|nerd face|geek", "1F9D0|face with monocle|curious",
      "1F615|confused face|confused", "1F61F|worried face|worried",
      "1F641|slightly frowning face|sad", "2639 FE0F|frowning face|sad",
      "1F62E|face with open mouth|surprised", "1F62F|hushed face|surprised",
      "1F632|astonished face|wow shocked", "1F633|flushed face|embarrassed",
      "1F97A|pleading face|please puppy eyes", "1F626|frowning face with open mouth|shocked",
      "1F627|anguished face|shocked", "1F628|fearful face|scared",
      "1F630|anxious face with sweat|nervous", "1F625|sad but relieved face|phew",
      "1F622|crying face|sad tear", "1F62D|loudly crying face|sob",
      "1F631|face screaming in fear|scream", "1F616|confounded face|frustrated",
      "1F623|persevering face|struggle", "1F61E|disappointed face|sad",
      "1F613|downcast face with sweat|sweat", "1F629|weary face|tired",
      "1F62B|tired face|tired", "1F971|yawning face|bored sleepy",
      "1F624|face with steam from nose|annoyed triumph", "1F621|pouting face|angry",
      "1F620|angry face|mad", "1F92C|face with symbols on mouth|swear",
      "1F608|smiling face with horns|devil", "1F480|skull|dead",
      "1F4A9|pile of poo|poop", "1F921|clown face|clown",
      "1F47B|ghost|boo", "1F47D|alien|ufo",
      "1F916|robot|bot", "1F648|see-no-evil monkey|monkey",
      "1F649|hear-no-evil monkey|monkey", "1F64A|speak-no-evil monkey|monkey"
   ]
},
{
   id: "people", label: "People", icon: "1F44B",
   items: [
      "1F44B|waving hand|hello hi bye wave", "1F91A|raised back of hand|hand",
      "270B|raised hand|stop high five", "1F596|vulcan salute|spock",
      "1F44C|OK hand|ok perfect", "270C FE0F|victory hand|peace",
      "1F91E|crossed fingers|luck", "1F91F|love-you gesture|love",
      "1F918|sign of the horns|rock", "1F919|call me hand|shaka call",
      "1F448|backhand index pointing left|left", "1F449|backhand index pointing right|right",
      "1F446|backhand index pointing up|up", "1F447|backhand index pointing down|down",
      "261D FE0F|index pointing up|up", "1F44D|thumbs up|yes like agree +1",
      "1F44E|thumbs down|no dislike -1", "270A|raised fist|fist",
      "1F44A|oncoming fist|punch fist bump", "1F44F|clapping hands|clap applause",
      "1F64C|raising hands|hooray celebrate", "1F450|open hands|hands",
      "1F932|palms up together|hands", "1F91D|handshake|deal agree",
      "1F64F|folded hands|please thanks pray", "270D FE0F|writing hand|write",
      "1F4AA|flexed biceps|strong muscle", "1F9E0|brain|smart think",
      "1F440|eyes|look see", "1F441 FE0F|eye|look see",
      "1F444|mouth|lips", "1F476|baby|baby",
      "1F9D2|child|kid", "1F466|boy|kid",
      "1F467|girl|kid", "1F9D1|person|adult",
      "1F468|man|adult", "1F469|woman|adult",
      "1F9D3|older person|old", "1F474|old man|old",
      "1F475|old woman|old", "1F46E|police officer|police",
      "1F477|construction worker|builder", "1F482|guard|guard",
      "1F575 FE0F|detective|spy", "1F934|prince|royal",
      "1F478|princess|royal", "1F385|Santa Claus|christmas",
      "1F9D9|mage|wizard magic", "1F9B8|superhero|hero",
      "1F9DF|zombie|undead", "1F937|person shrugging|shrug dunno",
      "1F926|person facepalming|facepalm", "1F64B|person raising hand|question hand",
      "1F647|person bowing|sorry bow", "1F481|person tipping hand|info",
      "1F645|person gesturing no|no", "1F646|person gesturing OK|ok",
      "1F483|woman dancing|dance", "1F57A|man dancing|dance",
      "1F6B6|person walking|walk", "1F3C3|person running|run",
      "1F5E3 FE0F|speaking head|speak talk", "1F464|bust in silhouette|user",
      "1F465|busts in silhouette|users group"
   ]
},
{
   id: "nature", label: "Animals and nature", icon: "1F436",
   items: [
      "1F436|dog face|dog puppy", "1F431|cat face|cat kitten",
      "1F42D|mouse face|mouse", "1F439|hamster|hamster",
      "1F430|rabbit face|bunny", "1F98A|fox|fox",
      "1F43B|bear|bear", "1F43C|panda|panda",
      "1F428|koala|koala", "1F42F|tiger face|tiger",
      "1F981|lion|lion", "1F42E|cow face|cow",
      "1F437|pig face|pig", "1F438|frog|frog",
      "1F435|monkey face|monkey", "1F414|chicken|chicken",
      "1F427|penguin|penguin", "1F426|bird|bird",
      "1F424|baby chick|chick", "1F986|duck|duck",
      "1F985|eagle|eagle", "1F989|owl|owl",
      "1F987|bat|bat", "1F43A|wolf|wolf",
      "1F417|boar|boar", "1F434|horse face|horse",
      "1F984|unicorn|unicorn", "1F41D|honeybee|bee",
      "1F41B|bug|bug", "1F98B|butterfly|butterfly",
      "1F40C|snail|slow", "1F41E|lady beetle|ladybug",
      "1F41C|ant|ant", "1F577 FE0F|spider|spider",
      "1F422|turtle|turtle slow", "1F40D|snake|snake",
      "1F98E|lizard|lizard", "1F996|T-Rex|dinosaur",
      "1F419|octopus|octopus", "1F991|squid|squid",
      "1F980|crab|crab", "1F420|tropical fish|fish",
      "1F41F|fish|fish", "1F42C|dolphin|dolphin",
      "1F433|spouting whale|whale", "1F988|shark|shark",
      "1F40A|crocodile|crocodile", "1F998|kangaroo|kangaroo",
      "1F418|elephant|elephant", "1F992|giraffe|giraffe",
      "1F993|zebra|zebra", "1F43E|paw prints|paws",
      "1F490|bouquet|flowers", "1F338|cherry blossom|flower",
      "1F339|rose|flower", "1F33B|sunflower|flower",
      "1F337|tulip|flower", "1F331|seedling|plant grow",
      "1F332|evergreen tree|tree", "1F333|deciduous tree|tree",
      "1F334|palm tree|tree", "1F335|cactus|cactus",
      "1F340|four leaf clover|luck", "1F341|maple leaf|leaf",
      "1F342|fallen leaf|autumn", "1F344|mushroom|mushroom",
      "1F30D|globe showing Europe-Africa|earth world", "1F30F|globe showing Asia-Australia|earth world",
      "1F319|crescent moon|moon night", "1F31E|sun with face|sun",
      "2600 FE0F|sun|sunny", "2B50|star|star",
      "1F31F|glowing star|star", "2728|sparkles|sparkle shiny",
      "26A1|high voltage|lightning", "1F525|fire|hot lit",
      "1F308|rainbow|rainbow", "2601 FE0F|cloud|cloudy",
      "26C5|sun behind cloud|cloudy", "1F327 FE0F|cloud with rain|rain",
      "26C8 FE0F|cloud with lightning and rain|storm", "2744 FE0F|snowflake|snow cold",
      "2603 FE0F|snowman|snow", "1F32A FE0F|tornado|storm",
      "1F30A|water wave|ocean sea", "1F4A7|droplet|water",
      "2614|umbrella with rain drops|rain"
   ]
},
{
   id: "food", label: "Food and drink", icon: "1F355",
   items: [
      "1F34E|red apple|fruit", "1F34F|green apple|fruit",
      "1F350|pear|fruit", "1F34A|tangerine|orange fruit",
      "1F34B|lemon|fruit", "1F34C|banana|fruit",
      "1F349|watermelon|fruit", "1F347|grapes|fruit",
      "1F353|strawberry|fruit", "1F352|cherries|fruit",
      "1F351|peach|fruit", "1F96D|mango|fruit",
      "1F34D|pineapple|fruit", "1F965|coconut|fruit",
      "1F95D|kiwi fruit|fruit", "1F345|tomato|vegetable",
      "1F951|avocado|vegetable", "1F346|eggplant|aubergine vegetable",
      "1F954|potato|vegetable", "1F955|carrot|vegetable",
      "1F33D|ear of corn|corn", "1F336 FE0F|hot pepper|chili spicy",
      "1F966|broccoli|vegetable", "1F9C4|garlic|vegetable",
      "1F9C5|onion|vegetable", "1F35E|bread|toast",
      "1F950|croissant|pastry", "1F968|pretzel|snack",
      "1F9C0|cheese wedge|cheese", "1F95A|egg|egg",
      "1F373|cooking|fried egg breakfast", "1F953|bacon|breakfast",
      "1F95E|pancakes|breakfast", "1F9C7|waffle|breakfast",
      "1F357|poultry leg|chicken", "1F356|meat on bone|meat",
      "1F354|hamburger|burger", "1F35F|french fries|chips",
      "1F355|pizza|pizza", "1F32D|hot dog|hotdog",
      "1F96A|sandwich|lunch", "1F32E|taco|mexican",
      "1F32F|burrito|mexican", "1F957|green salad|salad",
      "1F35D|spaghetti|pasta", "1F35C|steaming bowl|noodles ramen",
      "1F35B|curry rice|curry", "1F363|sushi|japanese",
      "1F371|bento box|lunch", "1F95F|dumpling|dumpling",
      "1F364|fried shrimp|prawn", "1F369|doughnut|donut",
      "1F36A|cookie|biscuit", "1F382|birthday cake|birthday",
      "1F370|shortcake|cake", "1F9C1|cupcake|cake",
      "1F967|pie|pie", "1F36B|chocolate bar|chocolate",
      "1F36C|candy|sweet", "1F36D|lollipop|sweet",
      "1F366|soft ice cream|icecream", "1F368|ice cream|icecream",
      "1F37F|popcorn|movie snack", "2615|hot beverage|coffee tea",
      "1F375|teacup without handle|tea", "1F964|cup with straw|soda drink",
      "1F9C3|beverage box|juice", "1F37A|beer mug|beer",
      "1F37B|clinking beer mugs|cheers beer", "1F942|clinking glasses|cheers toast",
      "1F377|wine glass|wine", "1F378|cocktail glass|cocktail",
      "1F379|tropical drink|cocktail", "1F37E|bottle with popping cork|champagne celebrate",
      "1F95B|glass of milk|milk", "1F9CA|ice|ice cube",
      "1F374|fork and knife|eat dinner", "1F944|spoon|eat"
   ]
},
{
   id: "activities", label: "Activities", icon: "26BD",
   items: [
      "26BD|soccer ball|football", "1F3C0|basketball|sport",
      "1F3C8|american football|sport", "26BE|baseball|sport",
      "1F3BE|tennis|sport", "1F3D0|volleyball|sport",
      "1F3C9|rugby football|sport", "1F3B1|pool 8 ball|billiards",
      "1F3D3|ping pong|table tennis", "1F3F8|badminton|sport",
      "1F3D2|ice hockey|sport", "26F3|flag in hole|golf",
      "1F3F9|bow and arrow|archery", "1F3A3|fishing pole|fishing",
      "1F94A|boxing glove|boxing", "1F94B|martial arts uniform|karate",
      "26F8 FE0F|ice skate|skating", "1F3BF|skis|ski",
      "1F3C4|person surfing|surf", "1F6B4|person biking|cycling",
      "1F3CA|person swimming|swim", "1F3C6|trophy|win winner",
      "1F947|1st place medal|gold first", "1F948|2nd place medal|silver second",
      "1F949|3rd place medal|bronze third", "1F3C5|sports medal|medal",
      "1F3AF|bullseye|target goal", "1F3AE|video game|gaming",
      "1F579 FE0F|joystick|gaming", "1F3B2|game die|dice",
      "265F FE0F|chess pawn|chess", "1F9E9|puzzle piece|puzzle",
      "1F3AD|performing arts|theatre", "1F3A8|artist palette|art paint",
      "1F3AC|clapper board|film movie", "1F3A4|microphone|karaoke sing",
      "1F3A7|headphone|music", "1F3BC|musical score|music",
      "1F3B5|musical note|music", "1F3B6|musical notes|music",
      "1F3B8|guitar|music", "1F3B9|musical keyboard|piano",
      "1F941|drum|music", "1F3BA|trumpet|music",
      "1F3BB|violin|music", "1F389|party popper|celebrate tada",
      "1F38A|confetti ball|celebrate", "1F388|balloon|party",
      "1F381|wrapped gift|present", "1F380|ribbon|bow",
      "1F384|Christmas tree|christmas", "1F383|jack-o-lantern|halloween",
      "1F386|fireworks|celebrate", "1F387|sparkler|celebrate",
      "1F9E8|firecracker|celebrate", "1F39F FE0F|admission tickets|tickets",
      "1F3AB|ticket|ticket"
   ]
},
{
   id: "travel", label: "Travel and places", icon: "1F697",
   items: [
      "1F697|automobile|car", "1F695|taxi|cab",
      "1F699|sport utility vehicle|car", "1F68C|bus|bus",
      "1F3CE FE0F|racing car|race", "1F693|police car|police",
      "1F691|ambulance|ambulance", "1F692|fire engine|fire truck",
      "1F69A|delivery truck|truck", "1F69C|tractor|farm",
      "1F6B2|bicycle|bike", "1F6F4|kick scooter|scooter",
      "1F3CD FE0F|motorcycle|motorbike", "1F682|locomotive|train steam",
      "1F686|train|train", "1F687|metro|subway train",
      "1F68A|tram|tram", "2708 FE0F|airplane|plane flight",
      "1F6EB|airplane departure|flight", "1F6EC|airplane arrival|flight",
      "1F681|helicopter|helicopter", "1F680|rocket|launch space",
      "1F6F8|flying saucer|ufo", "1F6A2|ship|boat",
      "26F5|sailboat|boat", "1F6A4|speedboat|boat",
      "2693|anchor|ship", "26FD|fuel pump|petrol gas",
      "1F6A6|vertical traffic light|traffic", "1F6A7|construction|roadworks",
      "1F5FA FE0F|world map|map", "1F9ED|compass|navigate",
      "1F3D4 FE0F|snow-capped mountain|mountain", "26F0 FE0F|mountain|mountain",
      "1F30B|volcano|volcano", "1F3D5 FE0F|camping|camp tent",
      "1F3D6 FE0F|beach with umbrella|beach", "1F3DD FE0F|desert island|island",
      "1F3E0|house|home", "1F3E1|house with garden|home",
      "1F3E2|office building|work office", "1F3E5|hospital|hospital",
      "1F3E6|bank|bank", "1F3E8|hotel|hotel",
      "1F3EB|school|school", "1F3ED|factory|factory",
      "1F3F0|castle|castle", "1F5FC|Tokyo tower|tower",
      "1F5FD|Statue of Liberty|statue", "26EA|church|church",
      "26F2|fountain|fountain", "1F3A1|ferris wheel|fair",
      "1F3A2|roller coaster|fair", "1F303|night with stars|night",
      "1F305|sunrise|morning", "1F307|sunset|evening",
      "1F309|bridge at night|bridge", "23F0|alarm clock|clock time",
      "231B|hourglass done|time", "23F3|hourglass not done|time wait"
   ]
},
{
   id: "objects", label: "Objects", icon: "1F4A1",
   items: [
      "1F4F1|mobile phone|phone", "1F4BB|laptop|computer",
      "1F5A5 FE0F|desktop computer|computer", "2328 FE0F|keyboard|typing",
      "1F5B1 FE0F|computer mouse|mouse", "1F4BE|floppy disk|save",
      "1F4BF|optical disk|cd", "1F4F7|camera|photo",
      "1F4F9|video camera|video", "1F3A5|movie camera|film",
      "1F4FA|television|tv", "1F4FB|radio|radio",
      "260E FE0F|telephone|phone call", "1F50B|battery|power",
      "1F50C|electric plug|power", "1F4A1|light bulb|idea",
      "1F526|flashlight|torch", "1F56F FE0F|candle|light",
      "1F4B0|money bag|money", "1F4B5|dollar banknote|money cash",
      "1F4B3|credit card|card pay", "1F48E|gem stone|diamond",
      "2696 FE0F|balance scale|law justice", "1F527|wrench|tool fix",
      "1F528|hammer|tool", "1F6E0 FE0F|hammer and wrench|tools",
      "2699 FE0F|gear|settings", "1F529|nut and bolt|tool",
      "1F9F0|toolbox|tools", "1F9F2|magnet|magnet",
      "1F52C|microscope|science", "1F52D|telescope|space",
      "1F4E1|satellite antenna|signal", "1F489|syringe|vaccine",
      "1F48A|pill|medicine", "1FA7A|stethoscope|doctor",
      "1F6AA|door|door", "1F6CF FE0F|bed|sleep",
      "1F6CB FE0F|couch and lamp|sofa", "1F6BD|toilet|bathroom",
      "1F6BF|shower|bathroom", "1F9F9|broom|clean",
      "1F9FA|basket|basket", "1F9FB|roll of paper|toilet paper",
      "1F9FC|soap|clean wash", "1F511|key|key",
      "1F5DD FE0F|old key|key", "1F512|locked|lock secure",
      "1F513|unlocked|unlock open", "1F4E6|package|parcel box",
      "1F4EB|closed mailbox with raised flag|mail", "1F4E7|e-mail|email",
      "2709 FE0F|envelope|mail letter", "1F4DD|memo|note write",
      "270F FE0F|pencil|write", "1F58A FE0F|pen|write",
      "1F4CE|paperclip|attach", "1F4CC|pushpin|pin",
      "1F4CD|round pushpin|pin location", "2702 FE0F|scissors|cut",
      "1F4C1|file folder|folder", "1F4C2|open file folder|folder",
      "1F4C4|page facing up|document", "1F4CB|clipboard|list",
      "1F4C5|calendar|date", "1F4C6|tear-off calendar|date",
      "1F4C8|chart increasing|up growth", "1F4C9|chart decreasing|down",
      "1F4CA|bar chart|stats", "1F4DA|books|library read",
      "1F4D6|open book|read", "1F4F0|newspaper|news",
      "1F516|bookmark|bookmark", "1F3F7 FE0F|label|tag",
      "1F50D|magnifying glass tilted left|search find", "1F514|bell|notification",
      "1F515|bell with slash|mute", "1F4E2|loudspeaker|announcement",
      "1F4E3|megaphone|announce", "1F4AC|speech balloon|chat comment",
      "1F4AD|thought balloon|think", "1F5E8 FE0F|left speech bubble|chat",
      "23F1 FE0F|stopwatch|time", "231A|watch|time",
      "1F6D2|shopping cart|shop", "1F45C|handbag|bag",
      "1F392|backpack|bag school", "1F9F3|luggage|travel suitcase",
      "1F453|glasses|spectacles", "1F454|necktie|tie",
      "1F455|t-shirt|shirt", "1F456|jeans|pants",
      "1F457|dress|dress", "1F45F|running shoe|shoe sneaker",
      "1F451|crown|king queen", "1F3A9|top hat|hat",
      "1F393|graduation cap|graduate", "2602 FE0F|umbrella|rain"
   ]
},
{
   id: "symbols", label: "Symbols", icon: "2764 FE0F",
   items: [
      "2764 FE0F|red heart|love", "1F9E1|orange heart|love",
      "1F49B|yellow heart|love", "1F49A|green heart|love",
      "1F499|blue heart|love", "1F49C|purple heart|love",
      "1F5A4|black heart|love", "1F90D|white heart|love",
      "1F90E|brown heart|love", "1F494|broken heart|sad",
      "2763 FE0F|heart exclamation|love", "1F495|two hearts|love",
      "1F49E|revolving hearts|love", "1F493|beating heart|love",
      "1F497|growing heart|love", "1F496|sparkling heart|love",
      "1F498|heart with arrow|love", "1F49D|heart with ribbon|love gift",
      "1F4AF|hundred points|100 perfect", "1F4A2|anger symbol|angry",
      "1F4A5|collision|boom", "1F4AB|dizzy|stars",
      "1F4A6|sweat droplets|water", "1F4A8|dashing away|fast",
      "1F4A4|zzz|sleep", "2705|check mark button|done yes",
      "2714 FE0F|check mark|tick done", "2611 FE0F|check box with check|done",
      "274C|cross mark|no wrong", "274E|cross mark button|no",
      "2795|plus|add", "2796|minus|subtract",
      "2797|divide|division", "2716 FE0F|multiply|times",
      "2757|red exclamation mark|important", "2753|red question mark|question",
      "2755|white exclamation mark|important", "2754|white question mark|question",
      "203C FE0F|double exclamation mark|important", "2049 FE0F|exclamation question mark|what",
      "26A0 FE0F|warning|caution", "1F6AB|prohibited|forbidden no",
      "26D4|no entry|forbidden", "1F6D1|stop sign|stop",
      "2B55|hollow red circle|circle", "1F534|red circle|circle",
      "1F7E0|orange circle|circle", "1F7E1|yellow circle|circle",
      "1F7E2|green circle|circle", "1F535|blue circle|circle",
      "1F7E3|purple circle|circle", "26AB|black circle|circle",
      "26AA|white circle|circle", "1F7E5|red square|square",
      "1F7E9|green square|square", "1F7E6|blue square|square",
      "2B1B|black large square|square", "2B1C|white large square|square",
      "1F536|large orange diamond|diamond", "1F537|large blue diamond|diamond",
      "1F53A|red triangle pointed up|up", "1F53B|red triangle pointed down|down",
      "27A1 FE0F|right arrow|arrow", "2B05 FE0F|left arrow|arrow",
      "2B06 FE0F|up arrow|arrow", "2B07 FE0F|down arrow|arrow",
      "2197 FE0F|up-right arrow|arrow", "2198 FE0F|down-right arrow|arrow",
      "1F504|counterclockwise arrows button|refresh", "1F503|clockwise vertical arrows|repeat",
      "1F519|BACK arrow|back", "1F51A|END arrow|end",
      "1F51B|ON! arrow|on", "1F51C|SOON arrow|soon",
      "1F51D|TOP arrow|top", "1F195|NEW button|new",
      "1F193|FREE button|free", "1F197|OK button|ok",
      "1F199|UP! button|up", "1F192|COOL button|cool",
      "1F198|SOS button|help", "2139 FE0F|information|info",
      "1F4F6|antenna bars|signal", "1F4F4|mobile phone off|off",
      "267B FE0F|recycling symbol|recycle", "269B FE0F|atom symbol|science",
      "262E FE0F|peace symbol|peace", "2622 FE0F|radioactive|danger",
      "2623 FE0F|biohazard|danger", "267E FE0F|infinity|forever",
      "00A9 FE0F|copyright|copyright", "00AE FE0F|registered|registered",
      "2122 FE0F|trade mark|trademark", "0023 FE0F 20E3|keycap number sign|hash",
      "0030 FE0F 20E3|keycap 0|zero", "0031 FE0F 20E3|keycap 1|one",
      "0032 FE0F 20E3|keycap 2|two", "0033 FE0F 20E3|keycap 3|three",
      "0034 FE0F 20E3|keycap 4|four", "0035 FE0F 20E3|keycap 5|five",
      "0036 FE0F 20E3|keycap 6|six", "0037 FE0F 20E3|keycap 7|seven",
      "0038 FE0F 20E3|keycap 8|eight", "0039 FE0F 20E3|keycap 9|nine",
      "1F51F|keycap 10|ten", "1F3C1|chequered flag|finish race",
      "1F6A9|triangular flag|flag", "1F3F3 FE0F|white flag|surrender",
      "1F3F4|black flag|flag"
   ]
} ];

const c_emoji_recent_max = 24;

function emoji_from_codes( codes )
{
   return String.fromCodePoint.apply( null, String( codes ).split( " " ).map( function( hex ) { return parseInt( hex, 16 ); } ) );
}

// NOTE: The categories with each entry read into { char, name, words }. Built once.
function emoji_catalogue( )
{
   return c_emoji_categories.map( function( category )
   {
      return {
         id: category.id,
         label: category.label,
         icon: emoji_from_codes( category.icon ),
         items: category.items.map( function( entry )
         {
            var parts = entry.split( "|" );

            return { char: emoji_from_codes( parts[ 0 ] ), name: parts[ 1 ], words: ( parts[ 1 ] + " " + ( parts[ 2 ] || "" ) ).toLowerCase( ) };
         } )
      };
   } );
}

// NOTE: Every word typed must start a word of the name or its extra words. Ranked in three
// tiers, each in catalogue order: the first word typed is a whole word of the name ("sun" -
// the sun, sun with face), then the name starts with it (sunflower), then the rest.
function search_emoji( catalogue, query )
{
   var typed = String( query || "" ).toLowerCase( ).trim( ).split( /\s+/ ).filter( function( w ) { return w !== ""; } );

   if( typed.length === 0 )
      return [ ];

   var exact = [ ], first = [ ], rest = [ ], seen = { };

   catalogue.forEach( function( category )
   {
      category.items.forEach( function( item )
      {
         if( seen[ item.char ] )
            return;

         var words = item.words.split( /[\s\-]+/ );

         var all = typed.every( function( t ) { return words.some( function( w ) { return w.indexOf( t ) === 0; } ); } );

         if( !all )
            return;

         seen[ item.char ] = true;

         var name = item.name.toLowerCase( );

         if( name.split( /[\s\-]+/ ).indexOf( typed[ 0 ] ) >= 0 )
            exact.push( item );
         else if( name.indexOf( typed[ 0 ] ) === 0 )
            first.push( item );
         else
            rest.push( item );
      } );
   } );

   return exact.concat( first, rest );
}

// NOTE: The most recent first, each once, and only so many.
function push_recent_emoji( recent, char )
{
   var list = ( recent || [ ] ).filter( function( c ) { return c !== char; } );

   list.unshift( char );

   return list.slice( 0, c_emoji_recent_max );
}

// NOTE: A stored list that is not a list of short strings is treated as empty.
function parse_recent_emoji( stored )
{
   try
   {
      var value = JSON.parse( stored );

      if( !Array.isArray( value ) )
         return [ ];

      return value.filter( function( c ) { return ( typeof c === "string" ) && ( c.length > 0 ) && ( c.length <= 16 ); } )
       .slice( 0, c_emoji_recent_max );
   }
   catch( e )
   {
      return [ ];
   }
}

if( typeof module !== "undefined" )
{
   module.exports = {
      c_emoji_categories: c_emoji_categories,
      c_emoji_recent_max: c_emoji_recent_max,
      emoji_from_codes: emoji_from_codes,
      emoji_catalogue: emoji_catalogue,
      search_emoji: search_emoji,
      push_recent_emoji: push_recent_emoji,
      parse_recent_emoji: parse_recent_emoji
   };
}
