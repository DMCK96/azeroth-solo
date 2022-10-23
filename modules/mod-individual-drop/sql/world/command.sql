DELETE FROM `command` WHERE name IN ('drop', 'drop set', 'drop view', 'drop default');

INSERT INTO `command` (`name`, `security`, `help`) VALUES 
('drop', 0, 'Syntax: .drop $subcommand\nType .help drop to see a list of subcommands\nor .help drop $subcommand to see info on the subcommand.'),
('drop set', 0, 'Syntax: .drop set $rate\nSet your custom drop rate.'),
('drop view', 0, 'Syntax: .drop view\nView your current drop rate.'),
('drop default', 0, 'Syntax: .drop default\nSet your custom drop rate to the default value'),
