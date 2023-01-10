struct Command {
    String name;
    String value;
    String error;
};

Command ParseCommand(String str) {
    Command cmd;
    int spacePos = str.indexOf(' ');
    if (spacePos < 0) cmd.error = "Не верная комманда";
    else {
        cmd.name = str.substring(0, spacePos);
        cmd.value = str.substring(spacePos+1, str.length());
    }

    if (cmd.name == "" || cmd.value == "") cmd.error = "Не верный формат";

    return cmd;
}