
require 'vt_parse_tables'

#
# check that for every state, there is a transition defined
# for every character between 0 and A0.
#

table = {}

anywhere_array = expand_ranges($states[:ANYWHERE])

$state_tables.each { |state, table|
    next if state == :ANYWHERE

    table.each_with_index { |val, i|
        if not (val or $state_tables[:ANYWHERE][i])
            raise "No transition defined from state #{state}, char 0x#{i.to_s(16)}!"
        end
    }
}

puts "Tables had all necessary transitions defined."

