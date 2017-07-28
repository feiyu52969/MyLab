class CreateFavorites < ActiveRecord::Migration[5.1]
  def change
    create_table :favorites do |t|
      t.integer :user_id
      t.string :story_id
      t.string :integer

      t.timestamps

      t.index :user_id
  	  t.index :story_id
  	  t.index :created_at
    end
  end
end
