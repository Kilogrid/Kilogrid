file = ARGV[0]

data_all = open(file).readlines.select {|line| line =~ /^0x/}.map {|line|
  line.chomp.to_i(16)
}

data_on_wall = data_all.select{|d| d & 0x10 > 0}.map{|d| d & 0x07}
data_in_wall = data_all.select{|d| d & 0x10 == 0}
data_all.map!{|d| d & 0x07}

labels = ["All", "On wall", "Inside"]

[data_all, data_on_wall, data_in_wall].each.with_index do |data, index|

  hash = {}

  data.sort.each do |d|
    hash[d] ||= 0
    hash[d] += 1
  end

  puts labels[index]
  hash.each do |k, v|
    puts "opinion #{k + 1} : #{v}\t(#{v / data.length.to_f})"
  end
  puts ""

end
