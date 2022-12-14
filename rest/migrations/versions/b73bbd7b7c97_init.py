"""Init

Revision ID: b73bbd7b7c97
Revises: 
Create Date: 2022-07-23 16:02:20.043860

"""
from alembic import op
import sqlalchemy as sa


# revision identifiers, used by Alembic.
revision = 'b73bbd7b7c97'
down_revision = None
branch_labels = None
depends_on = None


def upgrade():
    # ### commands auto generated by Alembic - please adjust! ###
    op.create_table('upload_session',
    sa.Column('id', sa.String(length=32), nullable=False),
    sa.Column('file_size', sa.BigInteger(), nullable=False),
    sa.Column('created_at', sa.DateTime(), nullable=True),
    sa.PrimaryKeyConstraint('id')
    )
    # ### end Alembic commands ###


def downgrade():
    # ### commands auto generated by Alembic - please adjust! ###
    op.drop_table('upload_session')
    # ### end Alembic commands ###
